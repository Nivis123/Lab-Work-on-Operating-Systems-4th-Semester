#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int global_var = 10;

int main() {

    int local_var = 20;
    
    printf("Parent process - global_var: address=%p, value=%d\n", &global_var, global_var);
    printf("Parent process - local_var: address=%p, value=%d\n", &local_var, local_var);
    
    printf("Parent process - PID: %d\n", getpid());
    
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }
    
    if (pid == 0) {

        printf("Child process - PID: %d, PPID: %d\n", getpid(), getppid());
        
        printf("Child process - global_var: address=%p, value=%d\n", &global_var, global_var);
        printf("Child process - local_var: address=%p, value=%d\n", &local_var, local_var);
        printf("Start change\n");
        global_var = 100;
        local_var = 200;
        printf("Child process - changed global_var: value=%d\n", global_var);
        printf("Child process - changed local_var: value=%d\n", local_var);
        sleep(300);
        exit(5);
    } else {

        sleep(1); 
        printf("Parent process - global_var after child changes: value=%d\n", global_var);
        printf("Parent process - local_var after child changes: value=%d\n", local_var);
        
        printf("Parent process - sleeping for 30 seconds...\n");
        
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            printf("Child process exited with code: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child process was terminated by signal: %d\n", WTERMSIG(status));
        } else {
            printf("Child process terminated abnormally OMG!!\n");
        }
        
        sleep(300);
    }
    
    return 0;
}

//kill -9 <PID>
//cat /proc/<>/smaps
//ps -o pid,ppid,state -pid
//ps -ef