#ifndef CGROUPMANAGER_H
#define CGROUPMANAGER_H
#include<string>
#define CGROUP_V1_PATH "/sys/fs/cgroup/cpu/my_cont"
#define CGROUP_V2_PATH "/sys/fs/cgroup/my_cont"

// void setup_cgroups();
class CgroupManager{
    public:
    CgroupManager();
    ~CgroupManager();
    void setupCgroups();
    private:
    std::string cgroupPath;
    void setCpuLimit(int);
    void removeCgroup();
};

#endif
