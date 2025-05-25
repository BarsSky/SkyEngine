[![sky engine build](https://github.com/BarsSky/SkyEngine/actions/workflows/.github-ci.yaml/badge.svg?branch=master)](https://github.com/BarsSky/SkyEngine/actions/workflows/.github-ci.yaml)

# Sky Engine


## Description

**Sky Engine** is an evolving 3D engine project aimed at exploring and demonstrating the use of the [Vulkan](https://vulkan.org/) graphics API.

The project supports multiple platforms:
- **Linux** (Debian-based images)
- **Windows** (MSVC and MinGW compilers supported)

---

## Dependencies

- [ImGui](https://github.com/ocornut/imgui) — for user interface
- [Vulkan API](https://vulkan.org/) — core rendering
- [GLFW](https://github.com/glfw/glfw) — cross-platform OS interaction
- [STB](https://github.com/nothings/stb.git) — textures and images
- [QT 5.14](https://www.qt.io/) — optional integration
- [Vulkan Examples](https://github.com/SaschaWillems/Vulkan) — learning materials

---

## Features

- Support for formats: **GLTF/GLB**, **OBJ**, and custom objects from vertex/index arrays
- Rendering of 3D and 2D objects with textures
- Particle system (CPU/GPU)
- Text rendering
- Volumetric lines using geometry shaders

---

## Roadmap

- [ ] Set up CI/CD auto-build for the project
  - [X] Build check for Windows
  - [ ] Build check for Linux/Debian
- [ ] Camera class with positioning at different scene points
- [ ] Scene format (json/xml, then binary)
- [X] Object cursor position detection via pixel ownership (shaders + main thread)
- [X] Primitive classes (line, circle, rectangle)
  - Line via vertex buffer, circle and rectangle as special cases
- [ ] Separately loadable resources for examples
  - [ ] Variations for different library build types
- [ ] Auto-generate documentation with Doxygen via CMake

---

## Screenshots and Examples

> _Screenshots and usage examples will be added in future versions._

![screenshot](../screenshots/test_screen.png)
---

## Contact & Support

- [Issues](https://github.com/BarsSky/SkyEngine/issues) — for bugs and suggestions
- [Discussions](https://github.com/BarsSky/SkyEngine/discussions) — for questions and community

---

**License:** MIT

---
