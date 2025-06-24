#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<time.h>

int global_initialized = 10;
int global_uninitialized;
const int global_const = 20;

void print_addresses() {
    int local_var = 30;
    static int static_var = 40;
    const int local_const = 50;

    printf("Local variable address: %p\n", (void*)&local_var);
    printf("Static variable address: %p\n", (void*)&static_var);
    printf("Local constant address: %p\n", (void*)&local_const);
}

int main() {

    printf("Global initialized variable address: %p\n", (void*)&global_initialized);
    printf("Global uninitialized variable address: %p\n", (void*)&global_uninitialized);
    printf("Global constant address: %p\n", (void*)&global_const);
    printf("pid %d\n",getpid());

    print_addresses();
    sleep(3000);

    return 0;
}

//b//
//cat /proc/<pid>/maps
//c//
//nm ./a | grep global_initialized
//nm ./a | grep global_uninitialized
//nm ./a | grep global_const
//nm ./a | grep static_var
//g//
//export MY_ENV_VAR="initial_value"
//i//
//echo $MY_ENV_VAR
