#pragma once

#include <memory>
#include <entt/fwd.hpp>
#include <entt/entt.hpp>
#include "GameComponents.hpp"
#include "EngineComponents.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class MovementSystem {
public:
    MovementSystem() {};

    MovementSystem(std::shared_ptr<entt::registry> entity_registry)
        : entity_registry(entity_registry) {
    }

    ~MovementSystem() {}

    void update(float deltaTime);

private:
    std::shared_ptr<entt::registry> entity_registry;
};
