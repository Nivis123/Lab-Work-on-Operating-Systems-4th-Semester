#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

size_t my_write(int fd, const void *buf, size_t count) {
    size_t result = syscall(SYS_write, fd, buf, count); 
    if (result < 0) {
    
        fprintf(stderr, "Error writing: %s\n", strerror(errno));//в errno сохранятеся ошибка
    }
    return result; 
}

int main() {
    const char *message = "Hello, world!\n";
    my_write(1, message, strlen(message)); 
    return 0;
}
