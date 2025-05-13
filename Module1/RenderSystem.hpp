#pragma once

#include <memory>
#include <entt/fwd.hpp>
#include <entt/entt.hpp>
#include "GameComponents.hpp"
#include "EngineComponents.hpp"
#include "ForwardRenderer.hpp"

class RenderSystem {
public:
    RenderSystem() {};

    RenderSystem(std::shared_ptr<entt::registry> entity_registry, eeng::ForwardRendererPtr renderer)
        : entity_registry(entity_registry), forwardRenderer(renderer) {
    }

    ~RenderSystem() {}

    void update(float time);

private:
    std::shared_ptr<entt::registry> entity_registry;
    eeng::ForwardRendererPtr forwardRenderer;
};
