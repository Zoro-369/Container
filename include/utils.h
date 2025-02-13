// #ifndef UTILS_H
// #define UTILS_H

// void create_directory(const char *path);
// bool is_directory_empty(const char *path);
// void enter_container();
// #endif

#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Utils {
    void createDirectory(const std::string& path);
    bool isDirectoryEmpty(const std::string& path);
    void writeToFile(const char* path, const char* data);
    
}
void enterContainer();
#endif // UTILS_H
