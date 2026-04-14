# Postman App — Qt6 Starter Template

A minimal, production-ready Qt6 Widgets desktop application in C++ using CMake.  
Designed as a starter template for a Postman-like API client.

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
│   └── MainWindow.h        # MainWindow class header
└── src/
    ├── main.cpp             # Application entry point
    └── MainWindow.cpp       # MainWindow implementation
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

MIT — use freely as a starting point for your own projects.
