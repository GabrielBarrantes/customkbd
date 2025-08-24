#pragma once
#include <string>

class InputDeviceInfo
{
public:
    // Fields
    std::string bus;
    std::string vendor;
    std::string product;
    std::string version;
    std::string name;
    std::string handlers;
    std::string eventNode;

    // Constructor: takes the path to the device, e.g., "/dev/input/event7"
    InputDeviceInfo(const std::string &devicePath);

    // Default constructor
    InputDeviceInfo() = default;

    // Print method
    void print() const;
    void printName() const;

private:
    void parseDeviceBlock(const std::string &deviceBlock);
};
