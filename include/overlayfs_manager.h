#ifndef OVERLAYFS_MANAGER_H
#define OVERLAYFS_MANAGER_H

#define LOWERDIR "../container_root/lowerdir"
#define UPPERDIR "../container_root/upperdir"
#define WORKDIR "../container_root/workdir"
#define MERGED "../container_root/merged"
#define CONTAINER "../container_root"
void setup_overlayfs();

#endif
