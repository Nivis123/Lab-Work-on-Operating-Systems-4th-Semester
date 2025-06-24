#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/types.h>


int main(){
    printf("PID этой программы: %d\n", getpid());
    int *ptr = (int*) malloc(100 * sizeof(int));
    static int a =10;
    while(1){ 
        int a=1;
    }

    return 0;
}