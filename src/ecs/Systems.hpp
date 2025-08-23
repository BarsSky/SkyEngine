#pragma once
#include <entt/entt.hpp>
#include "Components.hpp"

// Пример системы обновления позиции
inline void updatePosition(entt::registry& registry, float dt) {
    auto view = registry.view<Position, Velocity>();
    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& vel = view.get<Velocity>(entity);
        pos.x += vel.vx * dt;
        pos.y += vel.vy * dt;
        pos.z += vel.vz * dt;
    }
}
// Добавляйте свои системы здесь
