# Инструкция по сборке

В этом документе описано, как собрать SkyEngineBase из исходников.

## Необходимое ПО
- CMake 3.16+
- Компилятор с поддержкой C++17 (MSVC, MinGW, GCC, Clang)
- Vulkan SDK
- (Опционально) Qt 5.14+ для интеграции с Qt

## Шаги сборки
1. Клонируйте репозиторий с подмодулями:
   ```sh
   git clone --recursive <repo_url>
   ```
2. Сконфигурируйте проект:
   ```sh
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   ```
3. Соберите проект:
   ```sh
   cmake --build build --target install
   ```
4. (Опционально) Запустите тесты или примеры из папок `tests/` или `examples/`.

## Примечания
- Для Windows используйте подходящий генератор (например, "Visual Studio 17 2022" или "MinGW Makefiles").
- Для сборки с Qt укажите `-DQT_LIB_ENABLE=ON` и путь к Qt через `-DQT_PATH=...`.

---
