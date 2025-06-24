#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    printf("PID: %d\n", getpid());
    sleep(15);

    char *args[] = {"./self_exec", NULL};
    execvp(args[0], args);

    printf("Hello world\n");
    printf("PID: %d\n", getpid());
    return 0;
}