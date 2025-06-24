#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {  
        printf("Child process - PID: %d, PPID: %d\n", getpid(), getppid());
        printf("I'm zombie! UUUUUUu");
        exit(0);  
    } else {  
        printf("Parent process - PID: %d, Child PID: %d\n", getpid(), pid);
        printf("Parent process is sleeping for 60 seconds...\n");
        sleep(60);  
    }

    return 0;
}

//ps aux | grep -w Z