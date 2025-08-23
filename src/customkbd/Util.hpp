#pragma once
#include <string>
#include <sys/stat.h>

namespace ckbd
{
    inline bool file_exists(const std::string &p)
    {
        struct stat st{};
        return stat(p.c_str(), &st) == 0;
    }
}