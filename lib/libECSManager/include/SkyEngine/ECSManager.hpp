#pragma once
#include "Systems.hpp"
#include <cstddef>
#include <entt/entt.hpp>

class ECSManager {
public:
  ECSManager() { getPtr(this); }
  static ECSManager *getPtr(ECSManager *ptr = nullptr) {
    static ECSManager *mPtr = nullptr;
    if (mPtr == nullptr) {
      mPtr = ptr;
    }
    return mPtr;
  }

  entt::registry registry;

  // Методы для создания/удаления сущностей, компонентов, систем
};
