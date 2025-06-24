#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/types.h>

#define PAGE_SIZE sysconf(_SC_PAGESIZE)
#define PFN_MASK 0x7FFFFFFFFFFFFFULL
#define MAX_PAGES_TO_READ 100000

void print_pagemap(pid_t pid) {
    char pagemap_path[256], maps_path[256];
    sprintf(pagemap_path, "/proc/%d/pagemap", pid);
    sprintf(maps_path, "/proc/%d/maps", pid);

    FILE *maps = fopen(maps_path, "r");
    if (!maps) {
        perror("open maps");
        exit(EXIT_FAILURE);
    }

    int fd = open(pagemap_path, O_RDONLY);
    if (fd < 0) {
        perror("open pagemap");
        fclose(maps);
        exit(EXIT_FAILURE);
    }

    printf("Analyzing memory regions for PID %d...\n", pid);
    printf("Page Size: %ld bytes\n\n", PAGE_SIZE);

    uint64_t present_pages = 0;
    uint64_t swapped_pages = 0;
    uint64_t total_checked = 0;

    char line[256];
    while (fgets(line, sizeof(line), maps)) {
        uint64_t start, end;
        if (sscanf(line, "%" SCNx64 "-%" SCNx64, &start, &end) != 2)
            continue;

        uint64_t pages_in_region = (end - start) / PAGE_SIZE;
        uint64_t pagemap_offset = (start / PAGE_SIZE) * sizeof(uint64_t);

        if (lseek(fd, pagemap_offset, SEEK_SET) == (off_t)-1) {
            perror("lseek");
            continue;
        }

        printf("Region %" PRIx64 "-%" PRIx64 " (%" PRIu64 " pages):\n", 
               start, end, pages_in_region);

        for (uint64_t i = 0; i < pages_in_region && total_checked < MAX_PAGES_TO_READ; i++) {
            uint64_t entry;
            if (read(fd, &entry, sizeof(entry)) != sizeof(entry))
                break;

            total_checked++;
            if (entry & (1ULL << 63)) {
                uint64_t pfn = entry & PFN_MASK;
                printf("  Page %6" PRIu64 ": PFN %12" PRIu64 " (present)\n", 
                       i, pfn);
                present_pages++;
            } 
            else if (entry & (1ULL << 62)) {
                printf("  Page %6" PRIu64 ": in swap\n", i);
                swapped_pages++;
            }
        }
    }

    close(fd);
    fclose(maps);

    printf("\nMemory statistics for allocated regions:\n");
    printf("Total pages checked:    %" PRIu64 "\n", total_checked);
    printf("Present in RAM:         %" PRIu64 " (%.2f%%)\n", 
           present_pages, total_checked ? (double)present_pages/total_checked*100 : 0);
    printf("Swapped out:            %" PRIu64 " (%.2f%%)\n", 
           swapped_pages, total_checked ? (double)swapped_pages/total_checked*100 : 0);
    printf("Not allocated/zero:     %" PRIu64 " (%.2f%%)\n",
           total_checked - present_pages - swapped_pages,
           total_checked ? (double)(total_checked - present_pages - swapped_pages)/total_checked*100 : 0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        fprintf(stderr, "Current process PID: %d\n", getpid());
        return EXIT_FAILURE;
    }

    pid_t pid = atoi(argv[1]);
    if (pid <= 0) {
        fprintf(stderr, "Invalid PID. Must be positive integer.\n");
        return EXIT_FAILURE;
    }

    print_pagemap(pid);
    return EXIT_SUCCESS;
}
//#define PAGE_SIZE sysconf(_SC_PAGESIZE)
//#define PFN_MASK 0x7FFFFFFFFFFFFFULL  //55 бит 
//#define MAX_PAGES_TO_READ 100000  
//
//void print_pagemap(pid_t pid) {
//    char pagemap_path[256];
//    sprintf(pagemap_path, "/proc/%d/pagemap", pid);
//
//    int fd = open(pagemap_path, O_RDONLY);
//    if (fd < 0) {
//        perror("open pagemap");
//        exit(EXIT_FAILURE);
//    }
//
//    printf("Pagemap for PID %d:\n", pid);
//    printf("Page Size: %ld bytes\n", PAGE_SIZE);
//    printf("Reading first %d pages...\n\n", MAX_PAGES_TO_READ);
//
//    uint64_t entry;
//    uint64_t present_pages = 0;
//    uint64_t swapped_pages = 0;
//    uint64_t total_pages = 0;
//
//    while (read(fd, &entry, sizeof(entry)) == sizeof(entry) && 
//           total_pages < MAX_PAGES_TO_READ) {
//        
//        if (total_pages % 10000 == 0) {
//            printf("Processing page %" PRIu64 "...\r", total_pages);
//            fflush(stdout);
//        }
//
//        if (entry & (1ULL << 63)) {  
//            uint64_t pfn = entry & PFN_MASK;
//            printf("Page %6" PRIu64 ": PFN %12" PRIu64 " (present)\n", 
//                   total_pages, pfn);
//            present_pages++;
//        } 
//        else if (entry & (1ULL << 62)) {  
//            printf("Page %6" PRIu64 ": in swap\n", total_pages);
//            swapped_pages++;
//        }
//        
//        total_pages++;
//    }
//
//    close(fd);
//
//    printf("\n\nMemory statistics:\n");
//    printf("Total pages checked:    %" PRIu64 "\n", total_pages);
//    printf("Present in RAM:         %" PRIu64 " (%.2f%%)\n", 
//           present_pages, (double)present_pages/total_pages*100);
//    printf("Swapped out:            %" PRIu64 " (%.2f%%)\n", 
//           swapped_pages, (double)swapped_pages/total_pages*100);
//    printf("Not allocated/zero:     %" PRIu64 " (%.2f%%)\n",
//           total_pages - present_pages - swapped_pages,
//           (double)(total_pages - present_pages - swapped_pages)/total_pages*100);
//}
//
//int main(int argc, char *argv[]) {
//    if (argc != 2) {
//        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
//        fprintf(stderr, "Current process PID: %d\n", getpid());
//        return EXIT_FAILURE;
//    }
//
//    pid_t pid = atoi(argv[1]);
//    if (pid <= 0) {
//        fprintf(stderr, "Invalid PID. Must be positive integer.\n");
//        return EXIT_FAILURE;
//    }
//
//    print_pagemap(pid);
//    return EXIT_SUCCESS;
//}