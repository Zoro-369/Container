// /*
// system() - The system function in C/C++ is used to execute shell commands from within a program. It is part of the C standard library (stdlib.h).
// wget is a command-line utility used to download files from the internet. It supports protocols like HTTP, HTTPS, and FTP.
// chmod() - The chmod command is used to change file permissions in Linux/Unix systems.
// for symlinks read notes.
// */


#include "overlayFSManager.h"
#include "utils.h"
#include <iostream>
#include <sys/mount.h>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

OverlayFSManager::OverlayFSManager(const std::string& lowerDir, const std::string& upperDir,
                                   const std::string& workDir, const std::string& mergedDir)
    : lowerDir_(lowerDir), upperDir_(upperDir), workDir_(workDir), mergedDir_(mergedDir) {
        std::cout<<"Setting up OverlayFS..."<<std::endl;
        setup();
    }
OverlayFSManager::~OverlayFSManager() {
    unmountOverlay(); // Automatically cleans up when the object goes out of scope
}

void OverlayFSManager::setup() {
    createDirectories();
    downloadBusyBox();
    copyBusyBox();
    createBusyBoxSymlinks((lowerDir_+"/bin"));
    mountOverlay();
}



void OverlayFSManager::createDirectories(){
    Utils::createDirectory("./containerRoot");
    Utils::createDirectory(lowerDir_);
    Utils::createDirectory(lowerDir_+"/bin");
    Utils::createDirectory(workDir_);
    Utils::createDirectory(mergedDir_);
    Utils::createDirectory(upperDir_);

}


void OverlayFSManager::downloadBusyBox() {
    // Implement downloading logic
    if (Utils::isDirectoryEmpty((lowerDir_ + "/bin").c_str())) 
    {
        const char* busybox_url = "https://busybox.net/downloads/binaries/1.35.0-i686-linux-musl/busybox";
        std::cout << "Downloading BusyBox..." << std::endl;

        if (system(("wget -q " + std::string(busybox_url)).c_str()) != 0) {
            std::cerr << "Failed to download BusyBox!" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (chmod("busybox", 0755) == -1) {
            perror("chmod failed for BusyBox");
            exit(EXIT_FAILURE);
        }

        std::cout << "BusyBox downloaded and made executable." << std::endl;
    }
}

void OverlayFSManager::createBusyBoxSymlinks(const std::string& bin_path){
    std::cout << "Creating BusyBox symlinks..." << std::endl;

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
        std::string buffer;

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
            std::string symlink_path = bin_path + "/" + token;
            if (symlink("busybox", symlink_path.c_str()) == -1 && errno != EEXIST) {
                perror(("symlink failed for " + symlink_path).c_str());
            }
            token = strtok(nullptr, "\n");
        }

        std::cout << "Symlinks created successfully." << std::endl;
    }
}
void OverlayFSManager::copyBusyBox() {
    // Copy BusyBox into lowerdir/bin
        std::cout<<"Copying BusyBox into lowerdir/bin"<<std::endl;
        int src_fd = open("busybox", O_RDONLY);
        if (src_fd == -1) {
            perror("Failed to open BusyBox for reading");
            exit(EXIT_FAILURE);
        }

        int dest_fd = open((lowerDir_ + "/bin/busybox").c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0755);
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
        // create_busybox_symlinks(LOWERDIR "/bin");
}

void OverlayFSManager::mountOverlay(){
    // Prepare OverlayFS mount options
    std::string options = "lowerdir=" + lowerDir_+",upperdir="+ upperDir_+ ",workdir=" +workDir_;

    // Mount OverlayFS
    if (mount("overlay", mergedDir_.c_str(), "overlay", 0, options.c_str()) == -1) {
        perror("OverlayFS mount failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "OverlayFS mounted successfully at " << mergedDir_ << std::endl;
}

void OverlayFSManager::unmountOverlay() {
    std::cout << "Unmounting OverlayFS...\n";
    if (umount2(mergedDir_.c_str(), MNT_DETACH) == -1) {
        perror("Lazy umount failed");
    }
    // std::cout<<"Exited Container successfully ... \n";
}