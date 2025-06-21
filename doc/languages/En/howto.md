# How To Use SkyEngineBase

This guide provides typical usage scenarios and integration examples for SkyEngineBase.

## Basic Integration
1. Add SkyEngineBase as a submodule or dependency in your project.
2. Link against the library and include public headers from `lib/libSkyEngine/include/`.

## Initialization Example
```cpp
#include <SkyEngine/engine.h>

int main() {
    SkyEngine::Engine engine;
    engine.init();
    // ...
    engine.run();
    return 0;
}
```

## Loading a Model
```cpp
engine.loadModel("path/to/model.gltf");
```

## Customizing the Camera
```cpp
engine.getCamera().setPosition({0, 0, 10});
engine.getCamera().lookAt({0, 0, 0});
```

## UI Overlay Example
```cpp
engine.getUI().addText("Hello, SkyEngine!");
```

---
