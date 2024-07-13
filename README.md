# FlatOut 2 Map Zoom

Plugin to add some extra keybinds to FlatOut 2, such as changing the map zoom

Original code by FO2-Smiley, ported from a hex edit to a dll and then cleaned up by me :3

## Installation

- Make sure you have v1.2 of the game, as this is the only version this plugin is compatible with. (exe size of 2990080 bytes)
- Plop the files into your game folder, edit `FlatOut2MapZoom_gcp.toml` to change the options to your liking.
- Enjoy, nya~ :3

## Keybinds

- F1 - Change map zoom
- F2 - Toggle HUD

## Building

Building is done on an Arch Linux system with CLion and vcpkg being used for the build process. 

Before you begin, clone [nya-common](https://github.com/gaycoderprincess/nya-common) to a folder next to this one, so it can be found.

Required packages: `mingw-w64-gcc vcpkg`

To install all dependencies, use:
```console
vcpkg install tomlplusplus:x86-mingw-static
```

Once installed, copy files from `~/.vcpkg/vcpkg/installed/x86-mingw-static/`:

- `include` dir to `nya-common/3rdparty`
- `lib` dir to `nya-common/lib32`

You should be able to build the project now in CLion.
