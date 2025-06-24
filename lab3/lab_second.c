#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void create_directory(const char *path) {
    if (mkdir(path, 0755) == -1) {
        perror("mkdir");
    } else {
        printf("Directory created: %s\n", path);
    }
}

void list_directory(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}

void remove_directory(const char *path) {
    if (rmdir(path) == -1) {
        perror("rmdir");
    } else {
        printf("Directory removed: %s\n", path);
    }
}

void create_file(const char *path) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
    } else {
        close(fd);
        printf("File created: %s\n", path);
    }
}

void print_file(const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        perror("fopen");
        return;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file)) {
        printf("%s", buffer);
    }

    fclose(file);
}

void remove_file(const char *path) {
    if (unlink(path) == -1) {
        perror("unlink");
    } else {
        printf("File removed: %s\n", path);
    }
}

void create_symlink(const char *target, const char *link_path) {
    if (symlink(target, link_path) == -1) {
        perror("symlink");
    } else {
        printf("Symlink created: %s -> %s\n", link_path, target);
    }
}

void print_symlink(const char *path) {
    char buffer[1024];
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1);
    if (len == -1) {
        perror("readlink");
    } else {
        buffer[len] = '\0';
        printf("Symlink content: %s -> %s\n", path, buffer);
    }
}

void print_symlink_target(const char *path) {
    char buffer[1024];
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1);
    if (len == -1) {
        perror("readlink");
        return;
    }
    buffer[len] = '\0';

    print_file(buffer);
}

void remove_symlink(const char *path) {
    if (unlink(path) == -1) {
        perror("unlink");
    } else {
        printf("Symlink removed: %s\n", path);
    }
}

void create_hardlink(const char *target, const char *link_path) {
    if (link(target, link_path) == -1) {
        perror("link");
    } else {
        printf("Hardlink created: %s -> %s\n", link_path, target);
    }
}

void remove_hardlink(const char *path) {
    if (unlink(path) == -1) {
        perror("unlink");
    } else {
        printf("Hardlink removed: %s\n", path);
    }
}

void print_file_info(const char *path) {
    struct stat st;
    if (stat(path, &st) == -1) {
        perror("stat");
        return;
    }

    printf("File: %s\n", path);
    printf("Permissions: %o\n", st.st_mode & 0777);
    printf("Hard links: %lu\n", (unsigned long)st.st_nlink);
}

void change_permissions(const char *path, mode_t mode) {
    if (chmod(path, mode) == -1) {
        perror("chmod");
    } else {
        printf("Permissions changed for %s to %o\n", path, mode);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <action> <arguments...>\n", argv[0]);
        return 1;
    }

    char *action = strrchr(argv[0], '/');
    if (action) {
        action++;
    } else {
        action = argv[0];
    }

    if (strcmp(action, "create_directory") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s <directory_path>\n", action);
            return 1;
        }
        create_directory(argv[2]);
    } else if (strcmp(action, "list_directory") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s <directory_path>\n", action);
            return 1;
        }
        list_directory(argv[2]);
    } else if (strcmp(action, "remove_directory") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s <directory_path>\n", action);
            return 1;
        }
        remove_directory(argv[2]);
    } else if (strcmp(action, "create_file") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s <file_path>\n", action);
            return 1;
        }
        create_file(argv[2]);
    } else if (strcmp(action, "print_file") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s <file_path>\n", action);
            return 1;
        }
        print_file(argv[2]);
    } else if (strcmp(action, "remove_file") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s <file_path>\n", action);
            return 1;
        }
        remove_file(argv[2]);
    } else if (strcmp(action, "create_symlink") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Usage: %s <target> <link_path>\n", action);
            return 1;
        }
        create_symlink(argv[2], argv[3]);
    } else if (strcmp(action, "print_symlink") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s <link_path>\n", action);
            return 1;
        }
        print_symlink(argv[2]);
    } else if (strcmp(action, "print_symlink_target") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s <link_path>\n", action);
            return 1;
        }
        print_symlink_target(argv[2]);
    } else if (strcmp(action, "remove_symlink") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s <link_path>\n", action);
            return 1;
        }
        remove_symlink(argv[2]);
    } else if (strcmp(action, "create_hardlink") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Usage: %s <target> <link_path>\n", action);
            return 1;
        }
        create_hardlink(argv[2], argv[3]);
    } else if (strcmp(action, "remove_hardlink") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s <link_path>\n", action);
            return 1;
        }
        remove_hardlink(argv[2]);
    } else if (strcmp(action, "print_file_info") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s <file_path>\n", action);
            return 1;
        }
        print_file_info(argv[2]);
    } else if (strcmp(action, "change_permissions") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Usage: %s <file_path> <mode>\n", action);
            return 1;
        }
        mode_t mode = strtol(argv[3], NULL, 8);
        change_permissions(argv[2], mode);
    } else {
        fprintf(stderr, "Unknown action: %s\n", action);
        return 1;
    }

    return 0;
}
//ln -s целевой_файл символьная_ссылка
//./create_symlink . my_folder/my_file.txt my_link