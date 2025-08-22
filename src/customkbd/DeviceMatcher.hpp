#pragma once
#include <string>
#include <vector>

class DeviceMatcher
{
public:
    DeviceMatcher() = default;

    static std::vector<std::string> listInputDevices();
    static std::string eventnode(std::string const &name);

private:
};
