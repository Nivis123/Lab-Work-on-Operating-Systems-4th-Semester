#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void  __attribute__((noinline)) func(char **ptr){
    *ptr+=50;
}

void heap_operations() {
    
    char* buffer = (char*)malloc(100);
    if (buffer == NULL) {
        perror("Failed to allocate memory");
        return;
    }

    strcpy(buffer, "hello world");
    printf("Buffer content: %s\n", buffer);

    free(buffer);
    printf("Buffer content after free: %s\n", buffer);

    buffer = (char*)malloc(100);
    if (buffer == NULL) {
        perror("Failed to allocate memory");
        return;
    }

    strcpy(buffer, "hello world");
    printf("Buffer content: %s\n", buffer);


    char* mid_buffer=buffer;
    func(&mid_buffer);

    printf("Mid buffer content: %s\n", mid_buffer);

    free(mid_buffer);  

    printf("Buffer content after mid free: %s\n", buffer);

    free(buffer);
}

int main() {
    heap_operations();
    return 0;
}