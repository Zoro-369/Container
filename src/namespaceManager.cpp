#include "namespaceManager.h"
#include "container.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mount.h>

NamespaceManager::NamespaceManager() {
    std::cout << "Setting up namespaces...\n";
    setup();
}

NamespaceManager::~NamespaceManager() {
    std::cout << "NamespaceManager cleaned up.\n";
    // No explicit cleanup needed, namespaces disappear with the process
}

void NamespaceManager::setup() {
    std::cout << "Setting up namespaces...\n";
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
}

void NamespaceManager::forkAndEnterContainer() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) { // Child process (inside new PID namespace)
        Container container("./containerRoot/merged");
        container.run();
        exit(EXIT_SUCCESS);
    } else { // Parent
        wait(nullptr);
    }
    std::cout<<"Exited container successfully...\n";
}
