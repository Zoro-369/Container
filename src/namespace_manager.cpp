#include "namespace_manager.h"
#include <sys/mount.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include "overlayfs_manager.h"
#include "utils.h"

void setup_namespaces() {
    if (unshare(CLONE_NEWNS | CLONE_NEWPID) == -1) {
        perror("unshare failed");
        exit(EXIT_FAILURE);
    }

    // Make the mount namespace private
    if (mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL) == -1) {
        perror("Failed to make mount namespace private");
        exit(EXIT_FAILURE);
    }

    std::cout << "Namespaces set up. PID and mount namespace isolated.\n";
    // Fork to create a new process inside the PID namespace
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process (inside new PID namespace)

        enter_container();  // Move into the container
        exit(EXIT_SUCCESS);
    } else { 
        // parent process
        wait(nullptr);  // Wait for the container process to finish
        std::cout<<"Exited container successfully...\n";
    }

    
}

