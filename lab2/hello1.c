#include <stdio.h>
#include <sys/time.h>
#include <time.h>

int main(void) {
    struct timeval tv;
    int result = gettimeofday(&tv, NULL); 

    if (result == 0) {
        time_t seconds = tv.tv_sec;
        struct tm *tm_info = localtime(&seconds);

        printf("Current time: %04d-%02d-%02d %02d:%02d:%02d.%06ld\n",
               tm_info->tm_year + 1900, 
               tm_info->tm_mon + 1,     
               tm_info->tm_mday,
               tm_info->tm_hour,
               tm_info->tm_min,
               tm_info->tm_sec,
               tv.tv_usec);

        printf("Current time: %ld seconds and %ld microseconds since the Epoch.\n", tv.tv_sec, tv.tv_usec);
    } else {
        perror("gettimeofday failed");
    }
        
    
    return 0;
}

//strace -c wget kernel.org
//void* mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
//ssize_t write(int fd, const void *buf, size_t count);