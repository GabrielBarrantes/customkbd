# customkbd

A C++17 daemon that remaps a dedicated second keyboard on Linux. It identifies the chosen keyboard by name. It grabs that device exclusively and forwards keys (optionally injecting mapped combos) to the OS via `uinput`.

## Build

### Dependencies

Install `libevdev`, `libudev`, CMake >= 3.15, a C++17 compiler.

```bash
sudo apt update
sudo apt install -y libevdev-dev libudev-dev cmake g++
```

### Build and run development enviroment

Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Run

```bash
sudo ./build/customkbd list
sudo ./build/customkbd select 1
sudo ./build/customkbd status
sudo ./build/customkbd-daemon
```

### Build and install

```bash
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build
sudo systemctl enable customkbd.service
```

## Configuration and Usage

- Make sure to select a device before starting the service (only works if >1 keyboard present):

```bash
sudo customkbd list
sudo customkbd select 1
```

- Edit mappings at `/usr/local/etc/customkbd/mappings.json`:

```json
{
  "z": ["ctrl_down", "c", "ctrl_up"],
  "x": ["ctrl_down", "v", "ctrl_up"]
}
```

- After editing start or restart the service with

```bash
sudo systemctl start customkbd.service
sudo systemctl restart customkbd.service
```

## Security & Permissions

The daemon needs read access to `/dev/input/event*` and write to `/dev/uinput`. Default unit runs as root for simplicity. You can alternatively add a group and udev rules to restrict access.

## Notes

- If the previously used keyboard is not present at boot, the daemon idles and does nothing.
- The CLI only allows selecting a device when more than one keyboard is detected.
- For now hot plug detect is not supported, when a new keyboard is plugged, list, select and restart the service.
