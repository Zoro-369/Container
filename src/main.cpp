#include "namespaceManager.h"
#include "cgroupManager.h"
#include "overlayFSManager.h"
#include "utils.h"
#include <sys/mount.h>
#include<iostream>
#include <stdio.h>
#include <string>
int main()
{
    std::string lowerDir = "containerRoot/lowerdir";
    std::string upperDir = "containerRoot/upperdir";
    std::string workDir = "containerRoot/workdir";
    std::string mergedDir = "containerRoot/merged";

    try
    {
        // setup_overlayfs();
        OverlayFSManager overlay(lowerDir, upperDir, workDir, mergedDir);

        // Setup Cgroups
        CgroupManager cgManager;

        // Setup Namespaces
        NamespaceManager nsManager;
        // nsManager.setup();

        // Fork and enter the container
        nsManager.forkAndEnterContainer();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
