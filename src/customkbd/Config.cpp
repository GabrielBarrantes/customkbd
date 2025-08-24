#include "Config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

#include "DeviceMatcher.hpp"

using json = nlohmann::json;

RuntimeConfig Config::load(const ConfigPaths &paths)
{
    RuntimeConfig rc;

    {
        std::ifstream f(paths.device_json);
        if (!f)
        {
            std::cerr << "No device.json found; daemon will idle." << std::endl;
        }
        else
        {
            json j;
            f >> j;
            DeviceSelector sel;
            sel.path = DeviceMatcher::eventnode(j.value("name", ""));
            rc.selector = sel;
        }
    }

    {
        std::ifstream f(paths.mappings_json);
        if (!f)
        {
            std::cerr << "No mappings.json found; no remaps will be applied." << std::endl;
        }
        else
        {
            json j;
            f >> j;
            for (auto &[k, v] : j.items())
            {
                rc.mappings[k] = v.get<std::vector<std::string>>();
            }
        }
    }

    return rc;
}
