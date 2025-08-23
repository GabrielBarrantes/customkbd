#include "Keymap.hpp"
#include <linux/input-event-codes.h>
#include <algorithm>

namespace ckbd
{

    static std::string lower(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }

    int Keymap::keycode_from_token(const std::string &tok)
    {
        const auto t = lower(tok);
        if (t.size() == 1)
        {
            char c = t[0];
            if (c >= 'a' && c <= 'z')
                return KEY_A + (c - 'a');
            if (c >= '0' && c <= '9')
                return KEY_0 + (c - '0');
        }
        if (t == "ctrl" || t == "control")
            return KEY_LEFTCTRL;
        if (t == "shift")
            return KEY_LEFTSHIFT;
        if (t == "alt")
            return KEY_LEFTALT;
        if (t == "super" || t == "meta" || t == "win")
            return KEY_LEFTMETA;
        if (t == "space")
            return KEY_SPACE;
        if (t == "enter" || t == "return")
            return KEY_ENTER;
        if (t == "tab")
            return KEY_TAB;
        if (t == "esc" || t == "escape")
            return KEY_ESC;
        if (t == "backspace")
            return KEY_BACKSPACE;
        if (t == "left")
            return KEY_LEFT;
        if (t == "right")
            return KEY_RIGHT;
        if (t == "up")
            return KEY_UP;
        if (t == "down")
            return KEY_DOWN;
        // Extend with more tokens as needed
        return -1;
    }

} // namespace ckbd