#pragma once
#include <string>
#include <linux/uinput.h>

namespace ckbd
{

    class UInput
    {
    public:
        UInput();
        ~UInput();
        bool ok() const { return fd_ >= 0; }
        void emit_key(int keycode, int value); // value: 1=press, 0=release, 2=repeat
        void sync();

    private:
        int fd_ = -1;
    };

} // namespace ckbd