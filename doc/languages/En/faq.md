# FAQ

**Q:** How do I add a new module or object?

**A:** Create a new class in the appropriate folder (e.g., lib/libSkyEngine/extension/), add it to CMakeLists.txt, and document it with doxygen comments.

**Q:** How do I build the project with Qt support?

**A:** When configuring, set `-DQT_LIB_ENABLE=ON` and specify the Qt path with `-DQT_PATH=...`.

**Q:** Where can I find usage examples?

**A:** In the `examples/` and `tests/` folders.

**Q:** How do I generate Doxygen documentation?

**A:** Run Doxygen with the config from doc/doxygen.conf.in or use the corresponding CMake target.

---
