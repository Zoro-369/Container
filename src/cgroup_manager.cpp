#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "utils.h"

#define CGROUP_V1_PATH "/sys/fs/cgroup/cpu/my_cont"
#define CGROUP_V2_PATH "/sys/fs/cgroup/my_cont"

// Utility function to write to a file
void write_to_file(const char* path, const char* data) {
    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        perror(path);
        exit(EXIT_FAILURE);
    }
    if (write(fd, data, strlen(data)) == -1) {
        perror("write failed");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
}

// Check if cgroups v2 is active
int is_cgroup_v2() {
    struct stat buffer;
    return stat("/sys/fs/cgroup/cgroup.controllers", &buffer) == 0;
}

void setup_cgroups() {
    int cpu_limit_percentage = 50;
    if (cpu_limit_percentage <= 0 || cpu_limit_percentage > 100) {
        fprintf(stderr, "Invalid CPU limit: %d%% (must be between 1 and 100)\n", cpu_limit_percentage);
        exit(EXIT_FAILURE);
    }

    const char* cgroup_path = is_cgroup_v2() ? CGROUP_V2_PATH : CGROUP_V1_PATH;
    create_directory(cgroup_path);
    // if (mkdir(cgroup_path, 0755) == -1 && errno != EEXIST) {
    //     perror("mkdir failed");
    //     exit(EXIT_FAILURE);
    // }

    if (is_cgroup_v2()) {
        // Cgroups v2: Set CPU limit using cpu.max
        char cpu_max_path[256];
        snprintf(cpu_max_path, sizeof(cpu_max_path), "%s/cpu.max", cgroup_path);
        
        char quota_str[32];
        int max_quota = 100000 * cpu_limit_percentage / 100;  // 100ms period
        snprintf(quota_str, sizeof(quota_str), "%d 100000", max_quota);  // quota period

        write_to_file(cpu_max_path, quota_str);
    } else {
        // Cgroups v1: Set CPU limit using cpu.cfs_quota_us
        char cpu_quota_path[256];
        snprintf(cpu_quota_path, sizeof(cpu_quota_path), "%s/cpu.cfs_quota_us", cgroup_path);
        
        char quota_str[16];
        snprintf(quota_str, sizeof(quota_str), "%d", 1000 * cpu_limit_percentage);  // 100ms period

        write_to_file(cpu_quota_path, quota_str);
    }

    // Attach current process to the cgroup
    char cgroup_procs_path[256];
    snprintf(cgroup_procs_path, sizeof(cgroup_procs_path), "%s/cgroup.procs", cgroup_path);
    
    char pid_str[16];
    snprintf(pid_str, sizeof(pid_str), "%d", getpid());

    write_to_file(cgroup_procs_path, pid_str);

    printf("Cgroup '%s' created with %d%% CPU limit.\n", cgroup_path, cpu_limit_percentage);
}
