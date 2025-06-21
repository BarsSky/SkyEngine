# Как использовать SkyEngineBase

В этом руководстве приведены типовые сценарии использования и примеры интеграции SkyEngineBase.

## Базовая интеграция
1. Добавьте SkyEngineBase как подмодуль или зависимость в ваш проект.
2. Линкуйте с библиотекой и подключайте публичные заголовки из `lib/libSkyEngine/include/`.

## Пример инициализации
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

## Загрузка модели
```cpp
engine.loadModel("path/to/model.gltf");
```

## Настройка камеры
```cpp
engine.getCamera().setPosition({0, 0, 10});
engine.getCamera().lookAt({0, 0, 0});
```

## Пример UI Overlay
```cpp
engine.getUI().addText("Привет, SkyEngine!");
```

---
