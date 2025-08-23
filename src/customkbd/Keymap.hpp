#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace ckbd
{

    // Minimal key translator: maps strings like "z", "ctrl", "c" to Linux input key codes
    class Keymap
    {
    public:
        static int keycode_from_token(const std::string &t);
    };

} // namespace ckbd