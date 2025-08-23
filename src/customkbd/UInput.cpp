#include "UInput.hpp"
#include "Logger.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

namespace ckbd
{

    UInput::UInput()
    {
        fd_ = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
        if (fd_ < 0)
        {
            Logger::instance().error("Failed to open /dev/uinput");
            return;
        }

        ioctl(fd_, UI_SET_EVBIT, EV_KEY);
        for (int k = 0; k < 256; ++k)
            ioctl(fd_, UI_SET_KEYBIT, k);

        struct uinput_setup usetup{};
        snprintf(usetup.name, sizeof(usetup.name), "customkbd-virtual");
        usetup.id.bustype = BUS_USB;
        usetup.id.vendor = 0x1;  // dummy
        usetup.id.product = 0x1; // dummy
        usetup.id.version = 1;

        if (ioctl(fd_, UI_DEV_SETUP, &usetup) < 0)
        {
            Logger::instance().error("UI_DEV_SETUP failed");
            close(fd_);
            fd_ = -1;
            return;
        }
        if (ioctl(fd_, UI_DEV_CREATE) < 0)
        {
            Logger::instance().error("UI_DEV_CREATE failed");
            close(fd_);
            fd_ = -1;
            return;
        }
    }

    UInput::~UInput()
    {
        if (fd_ >= 0)
        {
            ioctl(fd_, UI_DEV_DESTROY);
            close(fd_);
        }
    }

    void UInput::emit_key(int keycode, int value)
    {
        if (fd_ < 0)
            return;
        input_event ev{};
        ev.type = EV_KEY;
        ev.code = keycode;
        ev.value = value;
        write(fd_, &ev, sizeof(ev));
    }

    void UInput::sync()
    {
        if (fd_ < 0)
            return;
        input_event ev{};
        ev.type = EV_SYN;
        ev.code = SYN_REPORT;
        ev.value = 0;
        write(fd_, &ev, sizeof(ev));
    }

} // namespace ckbd