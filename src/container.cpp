#include "container.h"
#include <iostream>
#include <unistd.h>
#include <sys/mount.h>
#include <cstdlib>
#include <sys/stat.h>

Container::Container(const std::string& rootfsPath) : rootfsPath_(rootfsPath) {}

void Container::run() {
    if (chroot(rootfsPath_.c_str()) == -1) {
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
    std::cout << "Inside container. Running shell...\n";
    const char* shell = "/bin/sh";
    char *const args[] = {"/bin/sh", nullptr};

    if (execvp(shell, args) == -1) {
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
}

    

    
    
    
