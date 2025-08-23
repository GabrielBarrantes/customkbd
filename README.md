# customkbd

A C++17 daemon that remaps a dedicated second keyboard on Linux. It identifies the chosen keyboard robustly (preferring `/dev/input/by-id` symlinks and falling back to a multi-attribute match: name, phys, uniq, bus, vendor, product, ID_PATH, ID_SERIAL). It grabs that device exclusively and forwards keys (optionally injecting mapped combos) to the OS via `uinput`.

## Build

Dependencies: `libevdev`, `libudev`, CMake >= 3.15, a C++17 compiler.

```bash
sudo apt-get install -y libevdev-dev libudev-dev cmake g++
mkdir build && cd build
cmake ..
make -j
```

## Install

```bash
sudo make install
sudo systemctl enable customkbd.service
sudo systemctl start customkbd.service
```

## Configure

- Select device (only works if >1 keyboard present):

```bash
sudo customkbd list
sudo customkbd select 1
```

This stores `/etc/customkbd/device.json` with a robust selector snapshot.

- Edit mappings at `/etc/customkbd/mappings.json`:

```json
{
  "z": ["ctrl_down", "c", "ctrl_up"],
  "x": ["ctrl_down", "v", "ctrl_up"]
}
```

> Tip: Use explicit `_down`/`_up` for modifiers when needed.

## Security & Permissions

The daemon needs read access to `/dev/input/event*` and write to `/dev/uinput`. Default unit runs as root for simplicity. You can alternatively add a group and udev rules to restrict access.

## Notes

- If the previously used keyboard is not present at boot, the daemon idles and does nothing.
- The CLI only allows selecting a device when more than one keyboard is detected.
- Matching prefers `/dev/input/by-id` symlink names which stay stable across reboots; when not available, a score-based fallback reduces false matches.
