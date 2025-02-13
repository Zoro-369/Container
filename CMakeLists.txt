cmake_minimum_required(VERSION 3.10)

project(ContainerRuntime)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(container
    src/main.cpp 
    src/overlayFSManager.cpp 
    src/namespaceManager.cpp 
    src/cgroupManager.cpp
    src/container.cpp 
    src/utils.cpp
)
