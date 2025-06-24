#include <stdio.h>
#include <stdlib.h>

int* get_local_var_address() {
    int local_var = 60;
    return &local_var;
}

int main() {
    int* addr = get_local_var_address();
    printf("Local variable address: %p\n", (void*)addr);
    return 0;
}