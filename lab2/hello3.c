#include <unistd.h>
#include <sys/syscall.h>

ssize_t my_write(int fd, const void *buf, size_t count) {
    return syscall(SYS_write, fd, buf, count);
}

int main() {
    const char *msg = "Hello, World";
    my_write(1, msg, 13); 
    return 0;
}
