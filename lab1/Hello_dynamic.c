#include <stdio.h>
#include <stdlib.h>


void hello_from_dynamic_lib();

int Global_data;

int main(void) {
    printf("Hello, world!\n");
    
    hello_from_dynamic_lib();

    return 0;
}
//gcc -shared -fPIC -o libdynamic_lib.so dynamic_lib.c
//gcc -o Hello_dynamic Hello_dynamic.c -L. -ldynamic_lib
//export LD_LIBRARY_PATH=.

