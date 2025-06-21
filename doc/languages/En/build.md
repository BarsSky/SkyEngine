# Build Instructions

This document describes how to build SkyEngineBase from source.

## Prerequisites
- CMake 3.16+
- C++17 compatible compiler (MSVC, MinGW, GCC, Clang)
- Vulkan SDK
- (Optional) Qt 5.14+ for Qt integration

## Build Steps
1. Clone the repository and its submodules:
   ```sh
   git clone --recursive <repo_url>
   ```
2. Configure the project:
   ```sh
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   ```
3. Build:
   ```sh
   cmake --build build --target install
   ```
4. (Optional) Run tests or examples from the `tests/` or `examples/` folders.

## Notes
- For Windows, use the appropriate generator (e.g., "Visual Studio 17 2022" or "MinGW Makefiles").
- For Qt builds, set `-DQT_LIB_ENABLE=ON` and specify `-DQT_PATH=...` if needed.

---
