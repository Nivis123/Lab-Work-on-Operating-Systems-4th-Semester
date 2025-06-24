#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PAGE_SIZE sysconf(_SC_PAGESIZE)
#define PFN_MASK 0x7FFFFFFFFFFFFFULL

int global_var = 10;  

void print_pagemap_info(pid_t pid, void *addr) {
    char pagemap_path[256];
    sprintf(pagemap_path, "/proc/%d/pagemap", pid);
    
    uint64_t page_start = (uint64_t)addr & ~(PAGE_SIZE - 1);
    uint64_t pagemap_offset = (page_start / PAGE_SIZE) * sizeof(uint64_t);
    
    int fd = open(pagemap_path, O_RDONLY);
    if (fd < 0) {
        perror("open pagemap");
        return;
    }
    
    if (lseek(fd, pagemap_offset, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        close(fd);
        return;
    }
    
    uint64_t entry;
    if (read(fd, &entry, sizeof(entry)) != sizeof(entry)) {
        perror("read");
        close(fd);
        return;
    }
    
    close(fd);
    
    printf("Memory info for address %p (PID %d):\n", addr, pid);
    printf("  Page start: 0x%" PRIx64 "\n", page_start);
    
    if (entry & (1ULL << 63)) {
        uint64_t pfn = entry & PFN_MASK;
        printf("  PFN: %" PRIu64 " (present in RAM)\n", pfn);
    } else if (entry & (1ULL << 62)) {
        printf("  Page is swapped out\n");
    } else {
        printf("  Page not present (zero or not allocated)\n");
    }
}

void print_with_delay(const char *message, int seconds) {
    printf("\n%s\n", message);
    fflush(stdout);
    sleep(seconds);
}

int main() {
    printf("Parent process - PID: %d\n", getpid());
    printf("Parent process - global_var: address=%p, value=%d\n", &global_var, global_var);
    
    print_with_delay("=== Before fork ===", 1);
    print_pagemap_info(getpid(), &global_var);
    
    print_with_delay("Forking in 5 seconds...", 5);
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }
    
    if (pid == 0) {
        print_with_delay("\n=== Child process started ===", 1);
        printf("Child PID: %d, PPID: %d\n", getpid(), getppid());
        printf("Child process - global_var before change: address=%p, value=%d\n",&global_var, global_var);

        print_with_delay("\n=== Before modification in child ===", 1);
        print_pagemap_info(getpid(), &global_var);
        
        print_with_delay("\nChild will modify variable in 5 seconds...", 5);
        
        global_var = 100;
        printf("\nChild process - global_var after change: value=%d\n", global_var);
        
        print_with_delay("\n=== After modification in child ===", 1);
        print_pagemap_info(getpid(), &global_var);
        
        print_with_delay("\nChild will sleep for 5 minutes for observation...", 1);
        sleep(300); 
        exit(0);
    } else {
        print_with_delay("\n=== Parent process continues ===", 1);
        
        print_with_delay("\nParent waiting for child to modify variable...", 5);
        
        printf("\nParent process - global_var after child changes: value=%d\n", global_var);
        
        print_with_delay("\n=== In parent after child modification ===", 1);
        print_pagemap_info(getpid(), &global_var);
        
        print_with_delay("\nParent waiting for child to terminate...", 1);
        waitpid(pid, NULL, 0);
    }
    
    return 0;
}