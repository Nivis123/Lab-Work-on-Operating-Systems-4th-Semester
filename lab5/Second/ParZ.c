#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {  
        printf("Child process - PID: %d, PPID: %d\n", getpid(), getppid());
        sleep(30);  
        printf("Child process - New PPID: %d (init)\n", getppid());  
        exit(0);
    } else {  
        printf("Parent process - PID: %d\n", getpid());
        exit(0);  
    }
}