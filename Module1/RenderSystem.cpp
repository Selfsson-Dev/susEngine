#include "RenderSystem.hpp"

void RenderSystem::update(float time)
{
    auto animView = entity_registry->view<AnimationComponent, MeshComponent>();
    static float blendingAmount = 0.0f;
    static int animationState = 0;

    auto view = entity_registry->view<TransformComponent, MeshComponent>();

    for (auto [entity, tfm, mesh] : view.each()) {
        forwardRenderer->renderMesh(mesh.resource, tfm.worldMatrix);
    }
}
