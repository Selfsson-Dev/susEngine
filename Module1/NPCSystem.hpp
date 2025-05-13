#pragma once

#include <memory>
#include <entt/fwd.hpp>
#include <entt/entt.hpp>
#include "GameComponents.hpp"
#include "EngineComponents.hpp"
#include <glm/glm.hpp>
#include "Log.hpp"

class NPCSystem {
public:
    NPCSystem() {};

    NPCSystem(std::shared_ptr<entt::registry> entity_registry)
        : entity_registry(entity_registry) {
    }

    ~NPCSystem() {}

    void update();

private:
    std::shared_ptr<entt::registry> entity_registry;
};
