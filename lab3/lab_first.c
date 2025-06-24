#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void reverse_string(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

void reverse_file_content(const char *src_path, const char *dst_path) {
    int src_fd = open(src_path, O_RDONLY);
    if (src_fd == -1) {
        perror("open src file");
        return;
    }

    struct stat st;
    if (fstat(src_fd, &st) == -1) {
        perror("fstat");
        close(src_fd);
        return;
    }
    off_t file_size = st.st_size;

    char *buffer = malloc(file_size);
    if (read(src_fd, buffer, file_size) != file_size) {
        perror("read");
        free(buffer);
        close(src_fd);
        return;
    }
    close(src_fd);

    for (off_t i = 0; i < file_size / 2; i++) {
        char temp = buffer[i];
        buffer[i] = buffer[file_size - i - 1];
        buffer[file_size - i - 1] = temp;
    }

    int dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd == -1) {
        perror("open dst file");
        free(buffer);
        return;
    }
    if (write(dst_fd, buffer, file_size) != file_size) {
        perror("write");
    }
    close(dst_fd);
    free(buffer);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return 1;
    }

    const char *src_dir = argv[1];

    char reversed_dir[256];
    strncpy(reversed_dir, src_dir, sizeof(reversed_dir));
    reverse_string(reversed_dir);

    if (mkdir(reversed_dir, 0755) == -1) {
        perror("mkdir");
        return 1;
    }

    DIR *dir = opendir(src_dir);
    if (!dir) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char src_path[512];
        snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);

        struct stat st;
        if (stat(src_path, &st) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISREG(st.st_mode)) {
            char reversed_name[256];
            strncpy(reversed_name, entry->d_name, sizeof(reversed_name));
            reverse_string(reversed_name);

            char dst_path[512];
            snprintf(dst_path, sizeof(dst_path), "%s/%s", reversed_dir, reversed_name);

            reverse_file_content(src_path, dst_path);
        }
    }

    closedir(dir);
    printf("Directory copied successfully: %s -> %s\n", src_dir, reversed_dir);
    return 0;
}

//./lab1  TEST 
//df -i
//stat lab1.c
//Суперблок df -hT /