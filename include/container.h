#ifndef CONTAINER_H
#define CONTAINER_H

#include <string>

class Container {
public:
    Container(const std::string& rootfsPath);
    void run();

private:
    std::string rootfsPath_;
};

#endif // CONTAINER_H
