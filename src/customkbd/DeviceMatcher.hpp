#pragma once
#include <string>
#include <vector>

class DeviceMatcher
{
public:
    DeviceMatcher() = default;

    std::vector<std::string> listInputDevices() const;

    // static helper
    static std::vector<std::string> enumerate()
    {
        DeviceMatcher matcher;
        return matcher.listInputDevices();
    }
};
