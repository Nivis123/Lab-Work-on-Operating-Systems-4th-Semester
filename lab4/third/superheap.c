#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdbool.h>

#define INITIAL_HEAP_SIZE (1024 * 1024)  
#define BLOCK_HEADER_SIZE sizeof(block_t)
#define MAX_ALLOC_SIZE (1024 * 1024 * 1024)  

typedef struct block {
    size_t size;
    bool free;
    struct block *next;
} block_t;

static block_t *heap_start = NULL;
static size_t total_heap_size = 0;

static int expand_heap(size_t size);
static void split_block(block_t *curr, size_t size);
static void merge_blocks();
void *my_malloc(size_t size);
void my_free(void *ptr);
void print_heap();
void print_stats();
void destroy_heap();

int main() {
    int *a=(int)malloc(sizeof(int));
    int *arr1 = (int *)my_malloc(10 * sizeof(int));
    int *arr2 = (int *)my_malloc(1000000 * sizeof(int));  
    
    if (!arr1 || !arr2) {
        printf("Allocation failed\n");
        return 1;
    }

    for (int i = 0; i < 10; i++) {
        arr1[i] = i;
    }

    for (int i = 0; i < 1000000; i++){
        arr2[i] = i;
    }

    printf("Heap status after allocations:\n");
    print_heap();
    print_stats();

    my_free(arr1);
    my_free(arr2);

    printf("Heap status after freeing:\n");
    print_heap();
    print_stats();

    destroy_heap();
    return 0;
}

static int expand_heap(size_t size) {
    size_t page_size = sysconf(_SC_PAGESIZE);
    size = ((size + page_size - 1) / page_size) * page_size;

    size = size < INITIAL_HEAP_SIZE ? INITIAL_HEAP_SIZE : size;

    void *new_region = mmap(NULL, size, PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (new_region == MAP_FAILED) {
        perror("mmap failed in expand_heap");
        return -1;
    }

    block_t *new_block = (block_t *)new_region;
    new_block->size = size - BLOCK_HEADER_SIZE;
    new_block->free = true;
    new_block->next = NULL;

    if (heap_start == NULL) {
        heap_start = new_block;
    } else {
        block_t *last = heap_start;
        while (last->next != NULL) last = last->next;
        last->next = new_block;
    }

    total_heap_size += size;
    return 0;
}

static void split_block(block_t *curr, size_t size) {
    if (curr == NULL || size == 0) return;
    
    if (curr->size <= size + BLOCK_HEADER_SIZE) return;
    
    block_t *new_block = (block_t *)((char *)curr + BLOCK_HEADER_SIZE + size);
    new_block->size = curr->size - size - BLOCK_HEADER_SIZE;
    new_block->free = true;
    new_block->next = curr->next;
    
    curr->size = size;
    curr->free = false;
    curr->next = new_block;
}

static void merge_blocks() {
    block_t *curr = heap_start;
    while (curr != NULL && curr->next != NULL) {
        if (curr->free && curr->next->free) {
            if ((char *)curr + BLOCK_HEADER_SIZE + curr->size == (char *)curr->next) {
                curr->size += BLOCK_HEADER_SIZE + curr->next->size;
                curr->next = curr->next->next;
                continue;  
            }
        }
        curr = curr->next;
    }
}

void *my_malloc(size_t size) {
    if (size == 0 || size > MAX_ALLOC_SIZE) return NULL;

    size = (size + 7) & ~7;

    if (heap_start == NULL && expand_heap(INITIAL_HEAP_SIZE) != 0) {
        return NULL;
    }

    block_t *curr = heap_start;
    while (curr != NULL) {
        if (curr->free && curr->size >= size) {
            if (curr->size > size + BLOCK_HEADER_SIZE + 8) { 
                split_block(curr, size);
            } else {
                curr->free = false;
            }
            return (void *)(curr + 1);  
        }
        curr = curr->next;
    }

    size_t required_size = size + BLOCK_HEADER_SIZE;
    if (expand_heap(required_size) != 0) {
        return NULL;
    }

    return my_malloc(size);
}

void my_free(void *ptr) {
    if (ptr == NULL) return;
    
    block_t *block = (block_t *)ptr - 1;
    block->free = true;
    
    merge_blocks(); 
}

void print_heap() {
    block_t *curr = heap_start;
    printf("\nHeap blocks:\n");
    printf("Total heap size: %zu bytes\n", total_heap_size);
    
    while (curr != NULL) {
        printf("Block at %p: size=%zu, free=%d\n",
              curr, curr->size, curr->free);
        curr = curr->next;
    }
    printf("\n");
}

void print_stats() {
    size_t used = 0, free = 0;
    block_t *curr = heap_start;
    
    while (curr != NULL) {
        if (curr->free) {
            free += curr->size;
        } else {
            used += curr->size;
        }
        curr = curr->next;
    }
    
    printf("Memory stats: Used=%zu (%.1f%%), Free=%zu (%.1f%%)\n",
          used, (double)used * 100 / total_heap_size,
          free, (double)free * 100 / total_heap_size);
}

void destroy_heap() {
    block_t *curr = heap_start;
    
    while (curr != NULL) {
        block_t *next = curr->next;

        munmap(curr, curr->size + BLOCK_HEADER_SIZE);

        curr = next;
    }
    
    heap_start = NULL;
    total_heap_size = 0;
}
