#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include "../customkbd/DeviceMatcher.hpp"
#include <nlohmann/json.hpp>
#include "../customkbd/InputDeviceInfo.hpp"

#ifdef DEBUG
const std::string DEVICE_JSON = "configs/device.json";
#else
const std::string DEVICE_JSON = "/etc/customkbd/device.json";
#endif

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " list | select <index> | status\n";
        return 1;
    }

    std::string cmd = argv[1];

    auto devices = DeviceMatcher::listInputDevices();

    if (cmd == "list")
    {
        int idx = 0;
        for (const auto &d : devices)
        {
            InputDeviceInfo dev(d);

            std::cout << std::setw(2) << idx++ << ": " << dev.name << std::endl;
        }
        return 0;
    }

    if (cmd == "select")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: select <index>\n";
            return 1;
        }

        int idx = std::stoi(argv[2]);
        if (idx < 0 || idx >= (int)devices.size())
        {
            std::cerr << "Invalid index\n";
            return 1;
        }

        nlohmann::json j;

        InputDeviceInfo dev(devices[idx]);

        j["path"] = devices[idx];
        j["vendor"] = dev.vendor;
        j["product"] = dev.product;
        j["version"] = dev.version;
        j["name"] = dev.name;
        j["eventNode"] = devices[idx];

        std::ofstream out(DEVICE_JSON);
        if (!out.is_open())
        {
            std::cerr << "Cannot write " << DEVICE_JSON << std::endl;
            return 1;
        }

        out << j.dump(2) << std::endl;
        std::cout << "Selected device: " << dev.name << std::endl;

        return 0;
    }

    if (cmd == "status")
    {
        std::ifstream in(DEVICE_JSON);
        if (!in.is_open())
        {
            std::cout << "No device selected\n";
            return 0;
        }

        nlohmann::json j;
        in >> j;
        std::cout << "Selected device: " << j["name"].get<std::string>() << std::endl;
        return 0;
    }

    std::cerr << "Unknown command\n";
    return 1;
}
