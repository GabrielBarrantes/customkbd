#include "DeviceMatcher.hpp"
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

std::vector<std::string> DeviceMatcher::listInputDevices() const
{
    std::ifstream infile("/proc/bus/input/devices");
    if (!infile)
    {
        std::cerr << "[ERROR] Cannot open /proc/bus/input/devices" << std::endl;
        return {};
    }

    std::vector<std::string> devices;

    std::string name;
    std::string handlers;
    std::string eventNode;
    bool hasKbd = false;
    bool evSupportsTyping = false;

    std::string line;
    while (std::getline(infile, line))
    {
        if (line.empty())
        {
            // end of one device block
            if (hasKbd && !eventNode.empty() && evSupportsTyping)
            {
                std::string lname = name;
                std::transform(lname.begin(), lname.end(), lname.begin(), ::tolower);

                if (lname.find("consumer") == std::string::npos &&
                    lname.find("system") == std::string::npos &&
                    lname.find("control") == std::string::npos &&
                    lname.find("virtual") == std::string::npos &&
                    lname.find("power") == std::string::npos &&
                    lname.find("video") == std::string::npos &&
                    lname.find("hid") == std::string::npos)
                {
                    devices.push_back("/dev/input/" + eventNode);
                }
            }

            // reset for next block
            name.clear();
            handlers.clear();
            eventNode.clear();
            hasKbd = false;
            evSupportsTyping = false;
            continue;
        }

        if (line.rfind("N:", 0) == 0)
        {
            name = line.substr(3); // Name="..."
        }
        else if (line.rfind("H:", 0) == 0)
        {
            auto pos = line.find("Handlers=");
            if (pos != std::string::npos)
            {
                handlers = line.substr(pos + 9);
                std::istringstream iss(handlers);
                std::string token;
                while (iss >> token)
                {
                    if (token.find("event") == 0)
                    {
                        eventNode = token;
                    }
                    if (token == "kbd")
                    {
                        hasKbd = true;
                    }
                }
            }
        }
        else if (line.rfind("B: EV=", 0) == 0)
        {
            // parse EV bitmap; 0x120013 indicates real keyboard (EV_KEY|EV_LED|EV_REP)
            std::string evHex = line.substr(6);
            unsigned long ev = std::stoul(evHex, nullptr, 16);
            if ((ev & 0x120013) == 0x120013)
            {
                evSupportsTyping = true;
            }
        }
    }

    // catch last block
    if (hasKbd && !eventNode.empty() && evSupportsTyping)
    {
        std::string lname = name;
        std::transform(lname.begin(), lname.end(), lname.begin(), ::tolower);

        if (lname.find("consumer") == std::string::npos &&
            lname.find("system") == std::string::npos &&
            lname.find("control") == std::string::npos &&
            lname.find("virtual") == std::string::npos &&
            lname.find("power") == std::string::npos &&
            lname.find("video") == std::string::npos &&
            lname.find("hid") == std::string::npos)
        {
            devices.push_back("/dev/input/" + eventNode);
        }
    }

    return devices;
}

std::string DeviceMatcher::eventnode(std::string const &name)
{
    std::ifstream infile("/proc/bus/input/devices");
    if (!infile.is_open())
    {
        return {};
    }

    std::string line;
    std::string currentName;
    std::string currentEvent;

    while (std::getline(infile, line))
    {
        if (line.rfind("N:", 0) == 0)
        {
            // Parse Name
            auto pos = line.find("Name=");
            if (pos != std::string::npos)
            {
                currentName = line.substr(pos + 5);
                if (!currentName.empty() && currentName.front() == '"')
                    currentName = currentName.substr(1, currentName.size() - 2);
            }
        }
        else if (line.rfind("H:", 0) == 0)
        {
            // Parse Handlers
            auto pos = line.find("Handlers=");
            if (pos != std::string::npos)
            {
                std::istringstream iss(line.substr(pos + 9));
                std::string handler;
                while (iss >> handler)
                {
                    if (handler.find("event") == 0)
                    {
                        currentEvent = "/dev/input/" + handler;
                    }
                }
            }
        }
        else if (line.empty())
        {
            // End of a block → check match
            if (!currentName.empty() && currentName == name && !currentEvent.empty())
            {
                return currentEvent;
            }
            currentName.clear();
            currentEvent.clear();
        }
    }

    // Check last block (in case no empty line at EOF)
    if (!currentName.empty() && currentName == name && !currentEvent.empty())
    {
        return currentEvent;
    }

    return {};
}