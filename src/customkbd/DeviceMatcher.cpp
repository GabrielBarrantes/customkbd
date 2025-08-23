#include "DeviceMatcher.hpp"
#include <dirent.h>
#include <iostream>

std::vector<std::string> DeviceMatcher::listInputDevices() const
{
    std::vector<std::string> devices;

    DIR *d = opendir("/dev/input");
    if (!d)
    {
        std::cerr << "Failed to open /dev/input directory\n";
        return devices;
    }

    struct dirent *entry;
    while ((entry = readdir(d)) != nullptr)
    {
        std::string name(entry->d_name);
        if (name.find("event") != std::string::npos)
        {
            devices.push_back("/dev/input/" + name);
        }
    }

    closedir(d); // ✅ inside a function
    return devices;
}
