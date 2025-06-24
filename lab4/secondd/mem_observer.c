#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#include <string.h>

#define PAGE_SIZE 4096
#define STACK_ALLOC_SIZE (8 * PAGE_SIZE)  
#define HEAP_ALLOC_SIZE (2 * 1024 * 1024) 
#define MMAP_SIZE (10 * PAGE_SIZE)        

void segfault_handler(int sig) {
    printf("\nCaught SIGSEGV (Segmentation Fault)!\n");
    exit(1);
}

void recursive_stack_alloc(int depth) {
    if (depth <= 0) return;
    
    char stack_array[STACK_ALLOC_SIZE];
    printf("Stack depth: %d, array at: %p\n", depth, stack_array);
    
    sleep(2);
    
    recursive_stack_alloc(depth - 1);
}

int main() {
    printf("=== Memory Observer ===\n");
    printf("PID: %d\n", getpid());
    printf("Waiting 10 seconds for monitoring\n");
    sleep(10);

    printf("\n=== Stack growth ===\n");
    recursive_stack_alloc(8);  
    sleep(3);

    printf("\n=== Heap growth ===\n");
    void *heap_blocks[5];
    for (int i = 0; i < 5; i++) {
        heap_blocks[i] = malloc(HEAP_ALLOC_SIZE);
        printf("Allocated heap block %d at %p\n", i+1, heap_blocks[i]);
        sleep(2);
    }

    printf("\n=== Freeing heap ===\n");
    for (int i = 0; i < 5; i++) {
        free(heap_blocks[i]);
        printf("Freed block %d\n", i+1);
        sleep(2);
    }

    printf("\n=== MMAP region ===\n");
    void *mmap_ptr = mmap(NULL, MMAP_SIZE, PROT_READ|PROT_WRITE, 
                         MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
    printf("Mapped 10 pages at %p\n", mmap_ptr);
    sleep(5);

    printf("\n=== Changing permissions ===\n");
    mprotect(mmap_ptr + 3*PAGE_SIZE, PAGE_SIZE, PROT_NONE);
    printf("Page 4 set to PROT_NONE (no access)\n");
    sleep(3);

    signal(SIGSEGV, segfault_handler);
    
    printf("Trying to read protected page...\n");
    //char test = *((char*)(mmap_ptr + 3*PAGE_SIZE)); 
    sleep(2);

    printf("\n=== Unmapping pages 4-6 ===\n");
    munmap(mmap_ptr + 3*PAGE_SIZE, 3*PAGE_SIZE);
    printf("Unmapped pages 4-6 at %p\n", mmap_ptr + 3*PAGE_SIZE);
    sleep(5);

    printf("\n=== Program finished ===\n");
    return 0;
}
//watch -d -n1 "cat /proc/self/maps | grep stack"
//watch -n 1 'cat /proc/<PID>/maps'
//watch -n 1 'cat /proc/36390/maps | grep -E "\[heap\]|\[stack\]|anon"'


//gcc -o mem_observer mem_observer.c && ./mem_observer
//watch -n 1 'cat /proc/$(pgrep mem_observer)/maps | grep -E "\[stack\]|\[heap\]|[anon]"'