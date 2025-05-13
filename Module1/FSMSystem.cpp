#include "FSMSystem.hpp"

void FSMSystem::update(float delta, float time, FSM& fsmTest)
{
    auto view = entity_registry->view<TransformComponent, MeshComponent, FSMComponent>();

    for (auto [entity, tfm, mesh, fsmc] : view.each()) {
        fsmc.timer += delta;

        if (fsmc.timer <= fsmc.cooldown) {
            fsmTest.transition_state(fsmc.currentIndex, false, mesh.resource);
        }
        else {
            fsmc.timer = 0.0f;
            fsmc.currentIndex = rand() % mesh.resource->getNbrAnimations();
        }

        ImGui::Begin("Debug for tester");
        ImGui::Text("timer for change : %f", fsmc.timer);
        ImGui::End();
    }
}