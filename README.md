# Arcade-GUI

A joystick controlled menu for my arcade cabinet

## Screenshot

![Isn't it pretty?](https://raw.githubusercontent.com/danwardvs/Arcade-GUI/master/screenshot.png)

## About

This program is a game launcher for an arcade cabinet. It lists games released on A.D.S. Games and installed MAME roms, downloads a game the first time it is picked, and shows the menu again when the game closes.

## A.D.S. Games

Games released on [A.D.S. Games](https://www.adsgames.net) for this platform show up in the menu. The list comes from `https://api.adsgames.net/games`, and the last list is kept for use offline. Launching a game fetches its newest release, downloads and unzips it, then runs it. Games are kept in the save folder, e.g. `~/Library/Application Support/adsgames/arcade-gui/games/<slug>/<version>/` on macOS, so each version downloads once.

## MAME

If [MAME](https://www.mamedev.org) is installed, rom sets it reports as good show up in the menu with their full names and title screens from [libretro-thumbnails](https://github.com/libretro-thumbnails/MAME). The rom folder is set in Settings (Tab, or Start on a controller). Left empty, MAME's own `rompath` from its `mame.ini` is used. Scanning runs in the background and is saved, so it only runs again when MAME or the rom folder changes. MAME keeps its settings and saves in the save folder, under `mame/`.

## Controls

Use a joystick to select a game and the action button to play.

## Building

Built with [asw](https://github.com/adsgames/asw). CMake fetches asw and its dependencies.

```
cmake --preset debug
cmake --build --preset debug
```

The program is at `build/debug/target/arcade-gui-2`.

### Linux

SDL builds from source and needs the system headers.

```
sudo apt install build-essential pkg-config libasound2-dev libpulse-dev libpipewire-0.3-dev libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev libxkbcommon-dev libwayland-dev wayland-protocols libdecor-0-dev libdrm-dev libgbm-dev libgl1-mesa-dev libegl1-mesa-dev libdbus-1-dev libudev-dev libibus-1.0-dev libssl-dev
```
