#include "cgroupManager.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdexcept>


namespace fs = std::filesystem;

// Check if cgroups v2 is active
int is_cgroup_v2() {
    struct stat buffer;
    return stat("/sys/fs/cgroup/cgroup.controllers", &buffer) == 0;
}



CgroupManager::CgroupManager(){
        cgroupPath = is_cgroup_v2() ? CGROUP_V2_PATH : CGROUP_V1_PATH;
        setupCgroups();
}

CgroupManager::~CgroupManager() {
    // removeCgroup();
}





void CgroupManager::removeCgroup() {
    std::cout << "[INFO] Removing cgroup: " << cgroupPath << std::endl;
    fs::remove_all(cgroupPath);
}
void CgroupManager::setupCgroups(){
    setCpuLimit(50);
}

void CgroupManager::setCpuLimit(int cpuLimitPercentage){
    if (cpuLimitPercentage <= 0 || cpuLimitPercentage > 100) {
        fprintf(stderr, "Invalid CPU limit: %d%% (must be between 1 and 100)\n", cpuLimitPercentage);
        exit(EXIT_FAILURE);
    }
    
    Utils::createDirectory(cgroupPath);
     if (is_cgroup_v2()) {
        // Cgroups v2: Set CPU limit using cpu.max
        char cpu_max_path[256];
        snprintf(cpu_max_path, sizeof(cpu_max_path), "%s/cpu.max", cgroupPath.c_str());
        
        char quota_str[32];
        int max_quota = 100000 * cpuLimitPercentage / 100;  // 100ms period
        snprintf(quota_str, sizeof(quota_str), "%d 100000", max_quota);  // quota period

        Utils::writeToFile(cpu_max_path, quota_str);
    } else {
        // Cgroups v1: Set CPU limit using cpu.cfs_quota_us
        char cpu_quota_path[256];
        snprintf(cpu_quota_path, sizeof(cpu_quota_path), "%s/cpu.cfs_quota_us", cgroupPath.c_str());
        
        char quota_str[16];
        snprintf(quota_str, sizeof(quota_str), "%d", 1000 * cpuLimitPercentage);  // 100ms period

        Utils::writeToFile(cpu_quota_path, quota_str);

    // Attach current process to the cgroup
    char cgroup_procs_path[256];
    snprintf(cgroup_procs_path, sizeof(cgroup_procs_path), "%s/cgroup.procs", cgroupPath.c_str());
    
    char pid_str[16];
    snprintf(pid_str, sizeof(pid_str), "%d", getpid());

    Utils::writeToFile(cgroup_procs_path, pid_str);

    printf("Cgroup '%s' created with %d%% CPU limit.\n", cgroupPath.c_str(), cpuLimitPercentage);
    }
}