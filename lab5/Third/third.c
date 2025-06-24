#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#define STACK_SIZE (65536)  // 64KB
#define RECURSION_DEPTH 10
#define FILENAME "clone_stack.bin"

void recursive_func(int depth) {
    if (depth <= 0) return;
    
    char str[32];
    snprintf(str, sizeof(str), "hello world %d", depth);
    
    printf("Child: depth=%d, str=%s, str_addr=%p\n", depth, str, str);
    recursive_func(depth - 1);
}

int child_func(void *arg) {
    printf("Child process started (PID=%d)\n", getpid());
    recursive_func(RECURSION_DEPTH);
    printf("Child process exiting\n");
    return 0;
}

int main() {
    int fd = open(FILENAME, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    if (ftruncate(fd, STACK_SIZE) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    
    void *stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (stack == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    
    printf("Parent: stack mapped at %p\n", stack);
    
    pid_t pid = clone(child_func, (char*)stack + STACK_SIZE,CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND, NULL);
    if (pid == -1) {
        perror("clone");
        exit(EXIT_FAILURE);
    }
    
    printf("Parent: child PID=%d created\n", pid);
    
    waitpid(pid, NULL, 0);
    
    msync(stack, STACK_SIZE, MS_SYNC);
    
    printf("\nAnalyzing file '%s':\n", FILENAME);
    
    FILE *f = fopen(FILENAME, "rb");
    if (!f) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    rewind(f);
    
    char *buffer = malloc(file_size);
    fread(buffer, 1, file_size, f);
    fclose(f);
    
    int hello_count = 0;
    for (long i = 0; i < file_size - 11; i++) {
        if (memcmp(buffer + i, "hello world", 11) == 0) {
            hello_count++;
            printf("Found 'hello world' at offset 0x%lx\n", i);
        }
    }
    printf("Total 'hello world' strings found: %d\n", hello_count);
    
    printf("\nSearching for recursion counter values:\n");
    for (long i = 0; i < file_size - sizeof(int); i++) {
        int val = *(int*)(buffer + i);
        if (val >= 1 && val <= RECURSION_DEPTH) {
            printf("Found value %d at offset 0x%lx\n", val, i);
        }
    }
    
    printf("\nSearching for return addresses:\n");
    for (long i = 0; i < file_size - sizeof(void*); i++) {
        void *addr = *(void**)(buffer + i);
        if ((long)addr >= 0x550000000000 && (long)addr < 0x570000000000) {
            printf("Found possible return address %p at offset 0x%lx\n", addr, i);
        }
    }
    
    free(buffer);
    munmap(stack, STACK_SIZE);
    close(fd);
    
    return 0;
}