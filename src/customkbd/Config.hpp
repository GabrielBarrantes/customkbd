#pragma once
#include <string>
#include <map>
#include <vector>
#include <optional>

struct DeviceSelector
{
    std::string path;
};

struct RuntimeConfig
{
    DeviceSelector selector;
    std::map<std::string, std::vector<std::vector<std::string>>> mappings;
};

struct ConfigPaths
{
    std::string device_json;
    std::string mappings_json;
};

class Config
{
public:
    Config() = default;

    RuntimeConfig load_complete(const ConfigPaths &paths);
};
