cmake_minimum_required(VERSION 3.10)

# Set project name
project(ContainerProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(src)

# Add the executable
add_executable(container src/main.cpp src/namespace_manager.cpp src/cgroup_manager.cpp src/overlayfs_manager.cpp src/utils.cpp)
