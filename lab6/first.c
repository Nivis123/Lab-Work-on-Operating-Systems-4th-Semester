#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <semaphore.h>

#define PAGE_SIZE sysconf(_SC_PAGESIZE)
#define SHM_NAME "/my_shared_mem"
#define SEM_NAME "/my_semaphore"

void* create_shared_memory() {
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    
    if (ftruncate(fd, PAGE_SIZE) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    
    void* ptr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    
    close(fd);
    return ptr;
}

void cleanup_shared_memory() {
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_NAME);
}

void writer_process() {
    unsigned int* ptr = (unsigned int*)create_shared_memory();
    sem_t* sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    
    unsigned int counter = 0;
    size_t num_elements = PAGE_SIZE / sizeof(unsigned int);
    
    printf("Writer started\n");
    
    while (1) {
        sem_wait(sem);
        
        for (size_t i = 0; i < num_elements; i++) {
            ptr[i] = counter++;
            if (counter == 0) counter = 0;
        }
        
        sem_post(sem);
        usleep(1000); 
    }
}

void reader_process() {
    unsigned int* ptr = (unsigned int*)create_shared_memory();
    sem_t* sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    
    unsigned int expected = 0;
    unsigned int prev_value = 0;
    int first_read = 1;
    size_t num_elements = PAGE_SIZE / sizeof(unsigned int);
    
    printf("Reader started\n");
    
    while (1) {
        sem_wait(sem);
        
        for (size_t i = 0; i < num_elements; i++) {
            unsigned int current = ptr[i];
            
            if (!first_read) {
                if (current != (prev_value + 1) && !(prev_value == UINT_MAX && current == 0)) {
                    printf("Sequence error: expected %u, got %u\n", prev_value + 1, current);
                }
            }
            
            prev_value = current;
            first_read = 0;
        }
        
        sem_post(sem);
        usleep(1000); 
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [writer|reader]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    if (strcmp(argv[1], "writer") == 0) {
        writer_process();
    } else if (strcmp(argv[1], "reader") == 0) {
        reader_process();
    } else {
        fprintf(stderr, "Invalid argument. Use 'writer' or 'reader'\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}