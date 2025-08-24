# customkbd

A C++17 daemon that remaps a dedicated second keyboard on Linux. It identifies the chosen keyboard robustly (preferring `/dev/input/by-id` symlinks and falling back to a multi-attribute match: name, phys, uniq, bus, vendor, product, ID_PATH, ID_SERIAL). It grabs that device exclusively and forwards keys (optionally injecting mapped combos) to the OS via `uinput`.

## Build

Dependencies: `libevdev`, `libudev`, CMake >= 3.15, a C++17 compiler.

```bash
sudo apt install -y libevdev-dev libudev-dev cmake g++
```

## Build and run development enviroment

```bash
mkdir build && cd build
cmake ..
make -j
sudo ./customkbd list
sudo ./customkbd status
sudo ./customkbd select 1
sudo ./customkbd-daemon
```

## Build and install

```bash
mkdir build && cd build
cmake ..
make -j
sudo make install
sudo systemctl enable customkbd.service
sudo systemctl start customkbd.service
```

## Usage

- Select device (only works if >1 keyboard present):

```bash
sudo customkbd list
sudo customkbd select 1
```

Edit mappings at `/etc/customkbd/mappings.json`:

```json
{
  "z": ["ctrl_down", "c", "ctrl_up"],
  "x": ["ctrl_down", "v", "ctrl_up"]
}
```

After editing restar the service with

```bash
sudo systemctl start customkbd.service
```

> Tip: Use explicit `_down`/`_up` for modifiers when needed.

## Security & Permissions

The daemon needs read access to `/dev/input/event*` and write to `/dev/uinput`. Default unit runs as root for simplicity. You can alternatively add a group and udev rules to restrict access.

## Notes

- If the previously used keyboard is not present at boot, the daemon idles and does nothing.
- The CLI only allows selecting a device when more than one keyboard is detected.
- For now hot plug detect is not supported, when a new keyboard is plugged list, select and restart the service.
