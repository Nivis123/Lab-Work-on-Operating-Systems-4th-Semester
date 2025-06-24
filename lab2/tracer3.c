#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>   
#include <sys/user.h> 
#include <sys/syscall.h> 
#include <signal.h>
#include "ptr.h"

void trace_child(pid_t child_pid) {
    int status;
    struct user_regs_struct regs;

    while (1) {
        wait(&status);

        if (WIFEXITED(status)) {
            break;
        }

        ptrace(PTRACE_GETREGS, child_pid, 0, &regs);

        long syscall_no = regs.orig_rax;
        const char *syscall_name = "unknown";
        if (syscall_no >= 0 && syscall_no < sizeof(syscall_names) / sizeof(syscall_names[0])) {
            syscall_name = syscall_names[syscall_no];
        }
        printf("System call: %ld (%s)\n", syscall_no, syscall_name);

        ptrace(PTRACE_SYSCALL, child_pid, 0, 0);
    }
}

void child_process(char *argv[]) {
    ptrace(PTRACE_TRACEME, 0, 0, 0);
    raise(SIGSTOP); 
    execvp(argv[0], argv);
    perror("execvp failed");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid_t child_pid = fork();

    if (child_pid == 0) {
        child_process(&argv[1]);
    } else {
        waitpid(child_pid, NULL, 0);
        ptrace(PTRACE_SETOPTIONS, child_pid, 0, PTRACE_O_TRACESYSGOOD);
        ptrace(PTRACE_SYSCALL, child_pid, 0, 0);
        trace_child(child_pid);
    }

    return EXIT_SUCCESS;
}