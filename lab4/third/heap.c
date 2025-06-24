#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdbool.h>

#define HEAP_SIZE (1024 * 1024)
#define BLOCK_HEADER_SIZE sizeof(block_t)

typedef struct block {
    size_t size;
    bool free;
    struct block *next;
} block_t;
//17 байт выравниваются до 24!!!

static block_t *heap_start = NULL;
static void *heap_end = NULL;

static int init_heap();
static void split_block(block_t *curr, size_t size);
static void merge_blocks();
void *my_malloc(size_t size);
void my_free(void *ptr);
void print_heap();

int main() 
{
    int *arr = (int *)my_malloc(10 * sizeof(int));
    if (arr == NULL) {
        printf("Allocation failed :(\n");
        return 1;
    }
    
    for (int i = 0; i < 10; i++) {
        arr[i] = i;
    }
    printf("Header size is: %zu\n", BLOCK_HEADER_SIZE);
    printf("Total heap is: %zu\n", (size_t)HEAP_SIZE);
    print_heap();
    
    my_free(arr);
    
    print_heap();
    
    return 0;
}

static int init_heap() 
{
    void *heap = mmap(NULL, HEAP_SIZE, PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);//пусть ядро само выберет адрес
    
    if (heap == MAP_FAILED) {
        perror("mmap failed error");
        return -1;
    }
    
    heap_start = (block_t *)heap;
    heap_start->size = HEAP_SIZE - BLOCK_HEADER_SIZE;
    heap_start->free = true;
    heap_start->next = NULL;
    
    heap_end = heap + HEAP_SIZE;
    
    return 0;
}

static void split_block(block_t *curr, size_t size) 
{
    if (curr == NULL || size == 0) return;
    
    block_t *new_block = (block_t *)((void *)curr + BLOCK_HEADER_SIZE + size);
    new_block->size = curr->size - size - BLOCK_HEADER_SIZE;
    new_block->free = true;
    new_block->next = curr->next;
    
    curr->size = size;
    curr->free = false;
    curr->next = new_block;
}

static void merge_blocks() 
{
    block_t *curr = heap_start;
    while (curr != NULL && curr->next != NULL) {
        if (curr->free && curr->next->free) {
            curr->size += BLOCK_HEADER_SIZE + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

void *my_malloc(size_t size) 
{
    if (size == 0) return NULL;
    
    if (heap_start == NULL) {
        if (init_heap() != 0) {
            return NULL;
        }
    }
    
    size = (size + 7) & ~7; //Выравнивание до 8 байт, тем что делаем кратынм 8
    
    block_t *curr = heap_start;
    while (curr != NULL) {
        if (curr->free && curr->size >= size) {
            if (curr->size > size + BLOCK_HEADER_SIZE) {
                split_block(curr, size);
            } else {
                curr->free = false;
            }
            return (void *)(curr + 1); 
        }
        curr = curr->next;
    }
    
    return NULL;
}

void my_free(void *ptr) 
{
    if (ptr == NULL) return;
    block_t *block = (block_t *)ptr - 1;
    block->free = true;
    merge_blocks();
}

void print_heap() 
{
    block_t *curr = heap_start;
    printf("Heap status is:\n");
    while (curr != NULL) {
        printf("Block at adress-%p: size=%zu, free=%d\n", 
               curr, curr->size, curr->free);
        curr = curr->next;
    }
    printf("\n");
}
