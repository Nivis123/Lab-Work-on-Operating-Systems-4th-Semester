#include <stdio.h>

void hello_from_dyn_runtime_lib() {
    printf("Hello from dynamic runtime library!\n");
}
//gcc -shared -fPIC -o libdynamic_lib.so dynamic_lib.c
