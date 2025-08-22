#pragma once
#include <string>

class InputDeviceInfo
{
public:
    std::string bus;
    std::string vendor;
    std::string product;
    std::string version;
    std::string name;
    std::string handlers;
    std::string eventNode;

    InputDeviceInfo(const std::string &devicePath);

    InputDeviceInfo() = default;

    void print() const;
    void printName() const;

private:
    void parseDeviceBlock(const std::string &deviceBlock);
};
