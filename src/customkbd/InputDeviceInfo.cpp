#include "InputDeviceInfo.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

InputDeviceInfo::InputDeviceInfo(const std::string &devicePath)
{
    std::ifstream infile("/proc/bus/input/devices");
    if (!infile)
    {
        std::cerr << "[ERROR] Cannot open /proc/bus/input/devices" << std::endl;
        return;
    }

    std::string line;
    std::string block;
    bool found = false;

    while (std::getline(infile, line))
    {
        if (line.empty())
        {
            if (!block.empty())
            {
                if (block.find(devicePath.substr(devicePath.find_last_of('/') + 1)) != std::string::npos)
                {
                    parseDeviceBlock(block);
                    found = true;
                    break;
                }
                block.clear();
            }
            continue;
        }
        block += line + "\n";
    }

    if (!found && !block.empty())
    {
        if (block.find(devicePath.substr(devicePath.find_last_of('/') + 1)) != std::string::npos)
        {
            parseDeviceBlock(block);
        }
    }
}

void InputDeviceInfo::parseDeviceBlock(const std::string &deviceBlock)
{
    std::istringstream infile(deviceBlock);
    std::string line;

    while (std::getline(infile, line))
    {
        if (line.empty())
            continue;

        if (line.rfind("I:", 0) == 0)
        {
            std::istringstream iss(line);
            std::string tmp;
            iss >> tmp;
            iss >> tmp;
            bus = tmp.substr(tmp.find("=") + 1);
            iss >> tmp;
            vendor = tmp.substr(tmp.find("=") + 1);
            iss >> tmp;
            product = tmp.substr(tmp.find("=") + 1);
            iss >> tmp;
            version = tmp.substr(tmp.find("=") + 1);
        }
        else if (line.rfind("N:", 0) == 0)
        {
            auto pos = line.find("Name=");
            if (pos != std::string::npos)
            {
                name = line.substr(pos + 5);
                if (!name.empty() && name.front() == '"')
                    name = name.substr(1, name.size() - 2);
            }
        }
        else if (line.rfind("H:", 0) == 0)
        {
            auto pos = line.find("Handlers=");
            if (pos != std::string::npos)
            {
                handlers = line.substr(pos + 9);
                std::istringstream iss(handlers);
                std::string handler;
                while (iss >> handler)
                {
                    if (handler.find("event") == 0)
                    {
                        eventNode = handler;
                    }
                }
            }
        }
    }
}

void InputDeviceInfo::print() const
{
    std::cout << "Device info:\n";
    std::cout << "  Bus: " << bus << "\n";
    std::cout << "  Vendor: " << vendor << "\n";
    std::cout << "  Product: " << product << "\n";
    std::cout << "  Version: " << version << "\n";
    std::cout << "  Name: " << name << "\n";
    std::cout << "  Handlers: " << handlers << "\n";
    std::cout << "  Event Node: " << eventNode << "\n";
}

void InputDeviceInfo::printName() const
{
    std::cout << "Device name:  ";
    std::cout << name << std::endl;
}
