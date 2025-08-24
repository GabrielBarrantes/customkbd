#pragma once
#include <string>
#include <map>
#include <vector>
#include <optional>

struct DeviceSelector
{
    std::string path; // path to the selected keyboard device
};

struct RuntimeConfig
{
    DeviceSelector selector;                                  // selected device
    std::map<std::string, std::vector<std::string>> mappings; // shortcut mappings
};

struct ConfigPaths
{
    std::string device_json;   // path to device.json
    std::string mappings_json; // path to mappings.json
};

class Config
{
public:
    Config() = default;

    RuntimeConfig load(const ConfigPaths &paths);
};
