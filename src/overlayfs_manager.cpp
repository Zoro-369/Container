/*
system() - The system function in C/C++ is used to execute shell commands from within a program. It is part of the C standard library (stdlib.h).
wget is a command-line utility used to download files from the internet. It supports protocols like HTTP, HTTPS, and FTP.
chmod() - The chmod command is used to change file permissions in Linux/Unix systems.
for symlinks read notes.
*/

#include "overlayfs_manager.h"
#include "utils.h"
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/wait.h>

using namespace std;

// Function to download BusyBox
void download_busybox() {
    const char* busybox_url = "https://busybox.net/downloads/binaries/1.35.0-i686-linux-musl/busybox";
    std::cout << "Downloading BusyBox..." << std::endl;

    if (system(("wget -q " + string(busybox_url)).c_str()) != 0) {
        std::cerr << "Failed to download BusyBox!" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (chmod("busybox", 0755) == -1) {
        perror("chmod failed for BusyBox");
        exit(EXIT_FAILURE);
    }

    cout << "BusyBox downloaded and made executable." << endl;
}

// Function to create symlinks 
void create_busybox_symlinks(const string& bin_path) {
    cout << "Creating BusyBox symlinks..." << endl;

    // Create a pipe to capture BusyBox command output
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // Child process
        close(pipefd[0]);  // Close read end
        dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to pipe
        execl((bin_path + "/busybox").c_str(), "busybox", "--list", nullptr);

        // If execl fails
        perror("execl failed");
        exit(EXIT_FAILURE);
    } else {  // Parent process
        close(pipefd[1]);  // Close write end
        char cmd[256];
        ssize_t bytes_read;
        string buffer;

        while ((bytes_read = read(pipefd[0], cmd, sizeof(cmd) - 1)) > 0) {
            cmd[bytes_read] = '\0';
            buffer += cmd;
        }
        close(pipefd[0]);

        // Wait for the child process to finish
        wait(nullptr);

        // Tokenize the output to create symlinks
        char* token = strtok(&buffer[0], "\n");
        while (token) {
            string symlink_path = bin_path + "/" + token;
            if (symlink("busybox", symlink_path.c_str()) == -1 && errno != EEXIST) {
                perror(("symlink failed for " + symlink_path).c_str());
            }
            token = strtok(nullptr, "\n");
        }

        cout << "Symlinks created successfully." << endl;
    }
}

// OverlayFS Setup
void setup_overlayfs() {
    cout << "Setting up OverlayFS..." << endl;

    // Create necessary directories
    cout<< "Creating directories for container ..."<<endl;
    create_directory(CONTAINER);
    create_directory(LOWERDIR);
    create_directory(LOWERDIR "/bin");
    create_directory(UPPERDIR);
    create_directory(WORKDIR);
    create_directory(MERGED);

    // Check if BusyBox is already present
    
    if (is_directory_empty(LOWERDIR "/bin")) {
        download_busybox();

        // Copy BusyBox into lowerdir/bin
        std::cout<<"Copying BusyBox into lowerdir/bin"<<std::endl;
        int src_fd = open("busybox", O_RDONLY);
        if (src_fd == -1) {
            perror("Failed to open BusyBox for reading");
            exit(EXIT_FAILURE);
        }

        int dest_fd = open(LOWERDIR "/bin/busybox", O_WRONLY | O_CREAT | O_TRUNC, 0755);
        if (dest_fd == -1) {
            perror("Failed to create BusyBox in container");
            close(src_fd);
            exit(EXIT_FAILURE);
        }

        // Copy BusyBox binary using read/write syscalls
        char buffer[4096];
        ssize_t bytes_read;
        while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
            if (write(dest_fd, buffer, bytes_read) != bytes_read) {
                perror("write failed");
                close(src_fd);
                close(dest_fd);
                exit(EXIT_FAILURE);
            }
        }

        close(src_fd);
        close(dest_fd);
        std::cout<<"Copied Successfully..."<<std::endl;

        // Create symlinks for BusyBox commands
        create_busybox_symlinks(LOWERDIR "/bin");
    }

    // Prepare OverlayFS mount options
    string options = "lowerdir=" LOWERDIR ",upperdir=" UPPERDIR ",workdir=" WORKDIR;

    // Mount OverlayFS
    if (mount("overlay", MERGED, "overlay", 0, options.c_str()) == -1) {
        perror("OverlayFS mount failed");
        exit(EXIT_FAILURE);
    }

    cout << "OverlayFS mounted successfully at " << MERGED << endl;
}
