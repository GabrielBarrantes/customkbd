#include "customkbd/Config.hpp"
#include "customkbd/InputDaemon.hpp"
#include "customkbd/Logger.hpp"
#include "customkbd/DeviceMatcher.hpp"
#include "customkbd/Util.hpp"
#include "customkbd/version.hpp"
#include <signal.h>
#include <iostream>

using namespace ckbd;

static void on_sigint(int) { _exit(0); }

int main()
{
    signal(SIGINT, on_sigint);
    signal(SIGTERM, on_sigint);

    Logger::instance().info(std::string("customkbd-daemon ") + customkbd::VERSION);

    ConfigPaths paths;

#ifdef DEBUG
    paths.device_json = "configs/device.json";
    paths.mappings_json = "configs/mappings.json";
#else
    paths.device_json = "/etc/customkbd/device.json";
    paths.mappings_json = "/etc/customkbd/mappings.json";
#endif

    Config cfgObj;
    RuntimeConfig cfg = cfgObj.load(paths);

    InputDaemon daemon(cfg.selector.path, cfg.mappings);

    if (!daemon.init())
    {
        std::cerr << "Failed to initialize InputDaemon\n";
        return 1;
    }

    daemon.run();

    return 0;
}
