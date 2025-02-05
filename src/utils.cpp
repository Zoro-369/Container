#include "utils.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include<dirent.h>
#include <iostream>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <cstring>
#include "overlayfs_manager.h"

void create_directory(const char *path) {
    if (mkdir(path, 0755) == -1 && errno != EEXIST) {
        perror("mkdir failed");
        exit(EXIT_FAILURE);
    }
}

bool is_directory_empty(const char *path) {
    DIR *dir = opendir(path);
    if (dir == nullptr) {
        return true;  // Consider the directory empty if it cannot be opened
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            closedir(dir);
            return false;  // Directory is not empty
        }
    }
    closedir(dir);
    return true;  // Directory is empty
}




void enter_container() {
    if (chroot(MERGED) == -1) {
        perror("chroot failed");
        exit(EXIT_FAILURE);
    }

    if (chdir("/") == -1) {
        perror("chdir failed");
        exit(EXIT_FAILURE);
    }
    // Ensure /proc exists
    mkdir("/proc", 0555);

    // Mount the proc filesystem
    if (mount("proc", "/proc", "proc", 0, NULL) == -1) {
        perror("Failed to mount /proc");
        exit(EXIT_FAILURE);
    }
    std::cout<<"proc mounted successfully\n";
    const char* shell = "/bin/sh";
    char *const args[] = {"/bin/sh", nullptr};

    if (execvp(shell, args) == -1) {
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
}



