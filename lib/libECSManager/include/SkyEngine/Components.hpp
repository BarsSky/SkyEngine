#pragma once
#include <entt/entt.hpp>
#include <optional>
#include <vector>

struct Position {
  float x, y, z;
};

struct Velocity {
  float vx, vy, vz;
};
// Добавляйте свои компоненты здесь
struct HierarchyComponent {
  std::optional<entt::entity> parent; // entity родителя (если есть)
  std::vector<entt::entity> children; // entity всех наследников (детей)
};
