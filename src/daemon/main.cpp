#include "customkbd/Config.hpp"
#include "customkbd/InputDaemon.hpp"
#include "customkbd/Logger.hpp"
#include "customkbd/DeviceMatcher.hpp"
#include "customkbd/Keymap.hpp"
#include "customkbd/UInput.hpp"
#include "customkbd/Util.hpp"
#include "customkbd/version.hpp"
#include <signal.h>
#include <iostream>

using namespace ckbd;

static void on_sigint(int) { _exit(0); }

int main()
{
    // Setup signal handlers
    signal(SIGINT, on_sigint);
    signal(SIGTERM, on_sigint);

    // Print version
    Logger::instance().info(std::string("customkbd-daemon ") + customkbd::VERSION);

    // Config paths (adjust to your config folder)
    ConfigPaths paths;
    paths.device_json = "../configs/device.json";
    paths.mappings_json = "../configs/mappings.json";

    // Create Config object and load runtime configuration
    Config cfgObj;
    RuntimeConfig cfg = cfgObj.load(paths);

    // Initialize and run daemon
    InputDaemon daemon(cfg.selector.path, cfg.mappings);

    if (!daemon.init())
    {
        std::cerr << "Failed to initialize InputDaemon\n";
        return 1;
    }

    daemon.run();

    return 0;
}
