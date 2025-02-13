#include "utils.h"
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/mount.h>

void Utils::createDirectory(const std::string& path) {
    if (mkdir(path.c_str(), 0755) == -1 && errno != EEXIST) {
        perror("mkdir failed");
        exit(EXIT_FAILURE);
    }
}

bool Utils::isDirectoryEmpty(const std::string& path) {
    DIR *dir = opendir(path.c_str());
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

// Utility function to write to a file
void Utils::writeToFile(const char* path, const char* data) {
    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        perror(path);
        exit(EXIT_FAILURE);
    }
    if (write(fd, data, strlen(data)) == -1) {
        perror("write failed");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
}


