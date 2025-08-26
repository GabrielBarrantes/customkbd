#pragma once
#include <string>
#include <map>
#include <vector>
#include <linux/input.h>

class InputDaemon
{
public:
    InputDaemon(const std::string &devicePath,
                const std::map<std::string, std::vector<std::vector<std::string>>> &mappings);

    ~InputDaemon();

    bool init();
    void run();
    void stop();

private:
    std::string devicePath;
    std::map<std::string, std::vector<std::vector<std::string>>> mappings;

    int fd{-1};
    int uinputFd{-1};
    bool running{false};

    void emitMapped(const std::vector<std::string> &actions);
    void emitMappedComplete(const std::vector<std::vector<std::string>> &commands);
    void forwardEvent(const struct input_event &ev);
};
