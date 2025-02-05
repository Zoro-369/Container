# Minimal Docker Clone

This project is a minimal Docker-like container runtime written in C++. It utilizes Linux namespaces, cgroups, and OverlayFS to create isolated container environments. The root filesystem is based on BusyBox for simplicity.

## Features

- **OverlayFS** for efficient layered file systems
- **Namespaces** for process and mount isolation
- **Chroot** to change the root filesystem for containerized processes
- Lightweight **BusyBox** environment

## Prerequisites

- Linux system with root access (required for namespace and mount operations)
- CMake (version 3.10 or higher)
- GCC or Clang compiler
- wget (for downloading BusyBox)

## Building the Project

1. **Clone the repository:**
   ```bash
   git clone https://github.com/Zoro-369/Container.git
   cd Container
   ```

2. **Create a build directory:**
   ```bash
   mkdir build && cd build
   ```

3. **Run CMake and build the project:**
   ```bash
   cmake ..
   make
   ```

4. **Run the container runtime (requires sudo):**
   ```bash
   sudo ./container
   ```

## Project Structure

- **main.cpp**: Entry point that initializes OverlayFS, namespaces, and container environment.
- **namespace_manager.cpp**: Handles PID and mount namespace isolation.
- **overlayfs_manager.cpp**: Sets up OverlayFS using BusyBox.
- **utils.cpp**: Utility functions for directory management and container entry.
- **CMakeLists.txt**: Build configuration for CMake.

