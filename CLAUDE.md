# brush_dragger

Qt 6 Widgets app, C++20, CMake + Ninja.

## Build
- Toolchain: MinGW 13.1 at D:/Apps/Qt/Tools/mingw1310_64. Don't use CLion's
  bundled MinGW (UCRT), because it doesn't match Qt's MSVCRT build.
- Configure with -DCMAKE_PREFIX_PATH=D:/Apps/Qt/6.11.3/mingw_64

## Git
- Don't add Co-Authored-By trailers to commit messages.
