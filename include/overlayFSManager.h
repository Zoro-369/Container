#ifndef OVERLAYFSMANAGER_H
#define OVERLAYFSMANAGER_H

#include <string>

class OverlayFSManager {
public:
    OverlayFSManager(const std::string& lowerDir, const std::string& upperDir,
                     const std::string& workDir, const std::string& mergedDir);
    ~OverlayFSManager();

private:
    std::string lowerDir_, upperDir_, workDir_, mergedDir_;
    void setup();
    void mountOverlay();
    void unmountOverlay();
    void downloadBusyBox();
    void copyBusyBox();
    void createBusyBoxSymlinks(const std::string& bin_path);
    void createDirectories();
};

#endif // OVERLAYFSMANAGER_H
