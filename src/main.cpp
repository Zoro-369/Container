#include "namespace_manager.h"
#include "cgroup_manager.h"
#include "overlayfs_manager.h"
#include "utils.h"
#include <sys/mount.h>
#include<stdio.h>

int main() {
    // Setup OverlayFS
   
    setup_overlayfs();
    
    // Setup Cgroups
    setup_cgroups();
    
    // Setup Namespaces
    setup_namespaces();

    // Enter the container environment
    // enter_container();

    // Cleanup (Unmount OverlayFS)
    if (umount2(MERGED, MNT_DETACH) == -1) {
    perror("Lazy umount failed");
}

    return 0;
}
