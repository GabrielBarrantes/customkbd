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
    {"z", KEY_Z},
    {"x", KEY_X},
    {"c", KEY_C},
    {"v", KEY_V},
    {"ctrl_down", KEY_LEFTCTRL},
    {"ctrl_up", KEY_LEFTCTRL}};

static const std::map<int, std::string> code_to_name_map = {
    {KEY_Z, "z"},
    {KEY_X, "x"},
    {KEY_C, "c"},
    {KEY_V, "v"},
    {KEY_LEFTCTRL, "ctrl"}};

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
    // Open the input device
    fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0)
    {
        std::cerr << "[ERROR] Failed to open device: " << devicePath << std::endl;
        return false;
    }

    // Grab the device exclusively (prevents OS from seeing keys)
    if (ioctl(fd, EVIOCGRAB, 1) < 0)
    {
        std::cerr << "[ERROR] Failed to grab device exclusively: " << devicePath << std::endl;
        close(fd);
        fd = -1;
        return false;
    }

    // Open uinput for sending virtual keys
    uinputFd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinputFd < 0)
    {
        std::cerr << "[ERROR] Failed to open /dev/uinput" << std::endl;
        ioctl(fd, EVIOCGRAB, 0); // release grab
        close(fd);
        fd = -1;
        return false;
    }

    // Enable key events and set key codes
    ioctl(uinputFd, UI_SET_EVBIT, EV_KEY);
    for (auto const &[name, code] : keymap)
        ioctl(uinputFd, UI_SET_KEYBIT, code);

    // Create uinput device
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

        // Check cooldown
        auto it_last = lastProcessed.find(ev.code);
        if (it_last != lastProcessed.end())
        {
            if (now - it_last->second < cooldown)
            {
                std::cout << "[DEBUG] Skipping duplicate key: " << ev.code << std::endl;
                continue; // skip this repeated event
            }
        }

        lastProcessed[ev.code] = now; // update last processed time

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
                continue; // suppress original key
            }
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
        auto it = keymap.find(a);
        if (it == keymap.end())
        {
            std::cout << "[WARN] Unknown mapping action: " << a << std::endl;
            continue;
        }

        struct input_event ev{};
        ev.type = EV_KEY;
        ev.code = it->second;

        if (a == "ctrl_down")
        {
            ev.value = 1;
            ctrl_pressed = true;
            std::cout << "[DEBUG] Pressing Ctrl" << std::endl;
        }
        else if (a == "ctrl_up")
        {
            ev.value = 0;
            ctrl_pressed = false;
            std::cout << "[DEBUG] Releasing Ctrl" << std::endl;
        }
        else
        {
            // press
            ev.value = 1;
            write(uinputFd, &ev, sizeof(ev));
            std::cout << "[DEBUG] Pressing key: " << a << std::endl;

            struct input_event syn{};
            syn.type = EV_SYN;
            syn.code = SYN_REPORT;
            syn.value = 0;
            write(uinputFd, &syn, sizeof(syn));
            // std::this_thread::sleep_for(std::chrono::milliseconds(5));

            // release
            ev.value = 0;
        }

        write(uinputFd, &ev, sizeof(ev));

        struct input_event syn{};
        syn.type = EV_SYN;
        syn.code = SYN_REPORT;
        syn.value = 0;
        write(uinputFd, &syn, sizeof(syn));
        // std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    if (ctrl_pressed)
    {
        struct input_event ev{};
        ev.type = EV_KEY;
        ev.code = keymap.at("ctrl_down");
        ev.value = 0;
        write(uinputFd, &ev, sizeof(ev));

        struct input_event syn{};
        syn.type = EV_SYN;
        syn.code = SYN_REPORT;
        syn.value = 0;
        write(uinputFd, &syn, sizeof(syn));
        std::cout << "[DEBUG] Ctrl released at end" << std::endl;
    }
}
