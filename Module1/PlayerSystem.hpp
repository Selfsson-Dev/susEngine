#pragma once

#include <memory>
#include <entt/fwd.hpp>
#include <entt/entt.hpp>
#include "GameComponents.hpp"
#include "EngineComponents.hpp"
#include <glm/glm.hpp>
#include "InputManager.hpp"
//#include "Game.hpp"
#include "FSM.h"

class PlayerSystem {
public:
    PlayerSystem() {};
    PlayerSystem(std::shared_ptr<entt::registry> entity_registry)
        : entity_registry(entity_registry) {
    }

    ~PlayerSystem() {}

    void update(InputManagerPtr input, float deltaTime, Camera& camera, FSM& fsm);

private:
    std::shared_ptr<entt::registry> entity_registry;
    //FSM fsm;
    //Camera camera;
};
