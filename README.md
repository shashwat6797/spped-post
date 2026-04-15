# SpeedPost

SpeedPost is a desktop native C++ API client built for a speedy experience.

## Prerequisites

- **Qt6** (Widgets module) — `qt6-base` on Arch Linux
- **CMake** ≥ 3.16
- **A C++17 compiler** (GCC 9+, Clang 10+)

```bash
# Arch Linux
sudo pacman -S qt6-base cmake base-devel
```

## Build & Run

```bash
mkdir -p build && cd build
cmake ..
make -j$(nproc)
./postman-app
```

## Project Structure

```
├── CMakeLists.txt          # Build configuration
├── README.md
├── include/
│   ├── core/               # Global providers
│   ├── common/             # Shared helpers
│   ├── modules/            # Feature-specific headers
│   └── ui/                 # UI section headers
└── src/
    ├── core/               # Provider implementations
    ├── common/             # Helper implementations
    ├── modules/            # Feature implementations
    ├── ui/                 # Window and UI section implementations
    └── main.cpp            # Application entry point
```

## Packaging (AppImage)

You can package the app as an AppImage using [linuxdeploy](https://github.com/linuxdeploy/linuxdeploy):

```bash
# 1. Install to a prefix
cmake --install build --prefix AppDir/usr

# 2. Create a .desktop file and icon in AppDir/usr/share/
#    (see freedesktop.org specs)

# 3. Run linuxdeploy
linuxdeploy --appdir AppDir \
  --plugin qt \
  --output appimage
```

This bundles all Qt libraries and produces a portable `.AppImage` file.

## License

MIT
