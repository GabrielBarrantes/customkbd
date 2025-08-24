#include "InputDaemon.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <linux/uinput.h>
#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <chrono>

static const std::map<std::string, int> keymap = {
    // letters
    {"a", KEY_A},
    {"b", KEY_B},
    {"c", KEY_C},
    {"d", KEY_D},
    {"e", KEY_E},
    {"f", KEY_F},
    {"g", KEY_G},
    {"h", KEY_H},
    {"i", KEY_I},
    {"j", KEY_J},
    {"k", KEY_K},
    {"l", KEY_L},
    {"m", KEY_M},
    {"n", KEY_N},
    {"o", KEY_O},
    {"p", KEY_P},
    {"q", KEY_Q},
    {"r", KEY_R},
    {"s", KEY_S},
    {"t", KEY_T},
    {"u", KEY_U},
    {"v", KEY_V},
    {"w", KEY_W},
    {"x", KEY_X},
    {"y", KEY_Y},
    {"z", KEY_Z},

    // numbers
    {"0", KEY_0},
    {"1", KEY_1},
    {"2", KEY_2},
    {"3", KEY_3},
    {"4", KEY_4},
    {"5", KEY_5},
    {"6", KEY_6},
    {"7", KEY_7},
    {"8", KEY_8},
    {"9", KEY_9},

    // modifiers
    {"ctrl_left", KEY_LEFTCTRL},
    {"ctrl_right", KEY_RIGHTCTRL},
    {"alt_left", KEY_LEFTALT},
    {"alt_right", KEY_RIGHTALT},
    {"shift_left", KEY_LEFTSHIFT},
    {"shift_right", KEY_RIGHTSHIFT},
    {"meta_left", KEY_LEFTMETA},
    {"meta_right", KEY_RIGHTMETA},

    {"up", KEY_UP},
    {"down", KEY_DOWN},
    {"left", KEY_LEFT},
    {"right", KEY_RIGHT},
    {"home", KEY_HOME},
    {"end", KEY_END},
    {"pageup", KEY_PAGEUP},
    {"pagedown", KEY_PAGEDOWN},
    {"space", KEY_SPACE},

    {"grave", KEY_GRAVE},
    {"enter", KEY_ENTER},
    {"escape", KEY_ESC},
    {"tab", KEY_TAB},
    {"capslock", KEY_CAPSLOCK},
    {"minus", KEY_MINUS},
    {"equal", KEY_EQUAL},

    // function keys
    {"f1", KEY_F1},
    {"f2", KEY_F2},
    {"f3", KEY_F3},
    {"f4", KEY_F4},
    {"f5", KEY_F5},
    {"f6", KEY_F6},
    {"f7", KEY_F7},
    {"f8", KEY_F8},
    {"f9", KEY_F9},
    {"f10", KEY_F10},
    {"f11", KEY_F11},
    {"f12", KEY_F12},

    // escape
    {"esc", KEY_ESC}};

static const std::map<int, std::string> code_to_name_map = {
    // letters
    {KEY_A, "a"},
    {KEY_B, "b"},
    {KEY_C, "c"},
    {KEY_D, "d"},
    {KEY_E, "e"},
    {KEY_F, "f"},
    {KEY_G, "g"},
    {KEY_H, "h"},
    {KEY_I, "i"},
    {KEY_J, "j"},
    {KEY_K, "k"},
    {KEY_L, "l"},
    {KEY_M, "m"},
    {KEY_N, "n"},
    {KEY_O, "o"},
    {KEY_P, "p"},
    {KEY_Q, "q"},
    {KEY_R, "r"},
    {KEY_S, "s"},
    {KEY_T, "t"},
    {KEY_U, "u"},
    {KEY_V, "v"},
    {KEY_W, "w"},
    {KEY_X, "x"},
    {KEY_Y, "y"},
    {KEY_Z, "z"},

    // numbers
    {KEY_0, "0"},
    {KEY_1, "1"},
    {KEY_2, "2"},
    {KEY_3, "3"},
    {KEY_4, "4"},
    {KEY_5, "5"},
    {KEY_6, "6"},
    {KEY_7, "7"},
    {KEY_8, "8"},
    {KEY_9, "9"},

    // modifiers
    {KEY_LEFTCTRL, "ctrl_left"},
    {KEY_RIGHTCTRL, "ctrl_right"},
    {KEY_LEFTALT, "alt_left"},
    {KEY_RIGHTALT, "alt_right"},
    {KEY_LEFTSHIFT, "shift_left"},
    {KEY_RIGHTSHIFT, "shift_right"},
    {KEY_LEFTMETA, "meta_left"},
    {KEY_RIGHTMETA, "meta_right"},

    {KEY_UP, "up"},
    {KEY_DOWN, "down"},
    {KEY_LEFT, "left"},
    {KEY_RIGHT, "right"},
    {KEY_HOME, "home"},
    {KEY_END, "end"},
    {KEY_PAGEUP, "pageup"},
    {KEY_PAGEDOWN, "pagedown"},
    {KEY_SPACE, "space"},

    {KEY_GRAVE, "grave"},
    {KEY_ENTER, "enter"},
    {KEY_ESC, "escape"},
    {KEY_TAB, "tab"},
    {KEY_CAPSLOCK, "capslock"},
    {KEY_MINUS, "minus"},
    {KEY_EQUAL, "equal"},

    // function keys
    {KEY_F1, "f1"},
    {KEY_F2, "f2"},
    {KEY_F3, "f3"},
    {KEY_F4, "f4"},
    {KEY_F5, "f5"},
    {KEY_F6, "f6"},
    {KEY_F7, "f7"},
    {KEY_F8, "f8"},
    {KEY_F9, "f9"},
    {KEY_F10, "f10"},
    {KEY_F11, "f11"},
    {KEY_F12, "f12"},

    // escape
    {KEY_ESC, "esc"}};

InputDaemon::InputDaemon(const std::string &devicePath_,
                         const std::map<std::string, std::vector<std::string>> &mappings_)
    : devicePath(devicePath_), mappings(mappings_)
{
}

InputDaemon::~InputDaemon()
{
    if (uinputFd >= 0)
    {
        ioctl(uinputFd, UI_DEV_DESTROY);
        close(uinputFd);
    }
    if (fd >= 0)
        close(fd);
}

bool InputDaemon::init()
{
    fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0)
    {
        std::cerr << "[ERROR] Failed to open device: " << devicePath << std::endl;
        return false;
    }

    if (ioctl(fd, EVIOCGRAB, 1) < 0)
    {
        std::cerr << "[ERROR] Failed to grab device exclusively: " << devicePath << std::endl;
        close(fd);
        fd = -1;
        return false;
    }

    uinputFd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinputFd < 0)
    {
        std::cerr << "[ERROR] Failed to open /dev/uinput" << std::endl;
        ioctl(fd, EVIOCGRAB, 0);
        close(fd);
        fd = -1;
        return false;
    }

    ioctl(uinputFd, UI_SET_EVBIT, EV_KEY);
    for (auto const &[name, code] : keymap)
    {
        ioctl(uinputFd, UI_SET_KEYBIT, code);
    }

    struct uinput_user_dev uidev{};
    memset(&uidev, 0, sizeof(uidev));
    strncpy(uidev.name, "customkbd", UINPUT_MAX_NAME_SIZE);
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor = 0x1234;
    uidev.id.product = 0x5678;
    uidev.id.version = 1;

    write(uinputFd, &uidev, sizeof(uidev));
    ioctl(uinputFd, UI_DEV_CREATE);

    std::cout << "[INFO] Device initialized and grabbed exclusively: " << devicePath << std::endl;
    return true;
}

void InputDaemon::run()
{
    running = true;
    struct input_event ev;

    // map key code -> last processed time
    std::unordered_map<int, std::chrono::steady_clock::time_point> lastProcessed;
    constexpr auto cooldown = std::chrono::milliseconds(20); // 20ms debounce

    while (running)
    {
        ssize_t n = read(fd, &ev, sizeof(ev));
        if (n != sizeof(ev))
            continue;
        if (ev.type != EV_KEY)
            continue;

        auto now = std::chrono::steady_clock::now();

        auto it_last = lastProcessed.find(ev.code);
        if (it_last != lastProcessed.end())
        {
            if (now - it_last->second < cooldown)
            {
                std::cout << "[DEBUG] Skipping duplicate key: " << ev.code << std::endl;
                continue;
            }
        }

        lastProcessed[ev.code] = now;

        std::string keyName;
        auto it_name = code_to_name_map.find(ev.code);
        if (it_name != code_to_name_map.end())
            keyName = it_name->second;

        std::cout << "[DEBUG] Key event: " << keyName
                  << " code=" << ev.code
                  << " value=" << ev.value << std::endl;

        if (ev.value == 1 && !keyName.empty())
        {

            auto it_map = mappings.find(keyName);
            if (it_map != mappings.end())
            {
                std::cout << "[DEBUG] Mapped key pressed: " << keyName << std::endl;
                emitMapped(it_map->second);
                continue;
            }

            continue;
        }

        std::cout << "[DEBUG] Forwarding Event: " << keyName << std::endl;
        forwardEvent(ev);
    }
}

void InputDaemon::stop()
{
    running = false;
}

void InputDaemon::forwardEvent(const struct input_event &ev)
{
    if (uinputFd >= 0)
    {
        write(uinputFd, &ev, sizeof(ev));

        struct input_event syn{};
        syn.type = EV_SYN;
        syn.code = SYN_REPORT;
        syn.value = 0;
        write(uinputFd, &syn, sizeof(syn));

        std::cout << "[DEBUG] Forwarded key: code=" << ev.code
                  << " value=" << ev.value << std::endl;
    }
}

void InputDaemon::emitMapped(const std::vector<std::string> &actions)
{
    bool ctrl_pressed = false;

    for (const auto &a : actions)
    {
        if (a.rfind("type:", 0) == 0)
        {
            std::string text = a.substr(5);
            std::cout << "[DEBUG] Typing string: " << text << std::endl;

            for (char ch : text)
            {
                // Convert character to lowercase name (e.g., 'h' -> "h")
                std::string keyname;
                if (ch == ' ')
                {
                    keyname = "space";
                }
                else
                {
                    keyname = std::string(1, std::tolower(ch));
                }

                auto it = keymap.find(keyname);
                if (it == keymap.end())
                {
                    std::cout << "[WARN] No mapping for char: " << ch << std::endl;
                    continue;
                }

                int code = it->second;

                struct input_event ev{};
                ev.type = EV_KEY;
                ev.code = code;

                // press
                ev.value = 1;
                write(uinputFd, &ev, sizeof(ev));

                struct input_event syn{};
                syn.type = EV_SYN;
                syn.code = SYN_REPORT;
                syn.value = 0;
                write(uinputFd, &syn, sizeof(syn));

                // release
                ev.value = 0;
                write(uinputFd, &ev, sizeof(ev));

                write(uinputFd, &syn, sizeof(syn));
            }

            continue;
        }

        auto it = keymap.find(a);
        if (it == keymap.end())
        {
            std::cout << "[WARN] Unknown mapping action: " << a << std::endl;
            continue;
        }

        struct input_event ev{};
        ev.type = EV_KEY;
        ev.code = it->second;
        ev.value = 1;
        write(uinputFd, &ev, sizeof(ev));
        std::cout << "[DEBUG] Pressing key: " << a << std::endl;
        // std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    struct input_event syn{};
    syn.type = EV_SYN;
    syn.code = SYN_REPORT;
    syn.value = 0;
    write(uinputFd, &syn, sizeof(syn));

    for (const auto &a : actions)
    {
        if (a.rfind("type:", 0) == 0)
        {
            continue;
        }

        auto it = keymap.find(a);
        if (it == keymap.end())
        {
            std::cout << "[WARN] Unknown mapping action: " << a << std::endl;
            continue;
        }

        struct input_event ev{};
        ev.type = EV_KEY;
        ev.code = it->second;
        ev.value = 0;
        write(uinputFd, &ev, sizeof(ev));
        std::cout << "[DEBUG] Releasing key: " << a << std::endl;
        // std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}
