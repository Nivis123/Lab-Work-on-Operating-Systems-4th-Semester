#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#define PAGE_SIZE 4096  

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        return 1;
    }

    char pagemap_path[256];
    snprintf(pagemap_path, sizeof(pagemap_path), "/proc/%s/pagemap", argv[1]);

   
    int pagemap_fd = open(pagemap_path, O_RDONLY);
    if (pagemap_fd == -1) {
        fprintf(stderr, "Failed to open %s: %s\n", pagemap_path, strerror(errno));
        return 1;
    }

    uint64_t entry;
    off_t offset = 0;

    while (read(pagemap_fd, &entry, sizeof(entry)) == sizeof(entry)) {
        if (entry & (1ULL << 63)) {
            uint64_t page_frame_number = entry & ((1ULL << 55) - 1);

            printf("Virtual page offset: 0x%lx, Physical page frame: 0x%lx\n",
                   (unsigned long)offset, (unsigned long)page_frame_number);
        } else {
            printf("Virtual page offset: 0x%lx, Page not present in memory\n",
                   (unsigned long)offset);
        }

        offset += PAGE_SIZE; 
    }

    close(pagemap_fd);

    return 0;
}