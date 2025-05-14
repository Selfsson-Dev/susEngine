#include "PlayerSystem.hpp"
#include "EventQueue.h"

void PlayerSystem::update(InputManagerPtr input, float deltaTime, Camera& camera, FSM& fsm)
{
     //get input
    using Key = eeng::InputManager::Key;
    bool W = input->IsKeyPressed(Key::W);
    bool A = input->IsKeyPressed(Key::A);
    bool S = input->IsKeyPressed(Key::S);
    bool D = input->IsKeyPressed(Key::D);
    bool spaceIsPressed = input->IsKeyPressed(Key::Space);

    //W = true;

    glm::vec3 forward = glm::vec3(glm_aux::R(camera.yaw, glm_aux::vec3_010) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    glm::vec3 right = glm::cross(forward, glm_aux::vec3_010);

    // now we know the way to set velocity to
    glm::vec3 dir = glm::vec3(forward * ((W ? 1.0f : 0.0f) + (S ? -1.0f : 0.0f))
        + right * ((A ? -1.0f : 0.0f) + (D ? 1.0f : 0.0f)));

    glm::vec3 dirNorm;

    if (glm::length(dir) < 0.05f) {
        dirNorm = glm_aux::vec3_000;
    }
    else {
        dirNorm = glm::normalize(dir);
    }

    auto view = entity_registry->view<TransformComponent, LinearVelocityComponent, PlayerControllerComponent, AnimationComponent, MeshComponent>();

    for (auto [entity, tfm, vel, player, anim, mesh] : view.each()) {
        //eeng::Log("normalized dir is: %f %f %f" , dirNorm.x, dirNorm.y, dirNorm.z);

        if (spaceIsPressed && player.jumpTimer <= player.jumpDuration) {
            player.isJumping = true;
        }

        if (player.jumpTimer >= player.jumpDuration) {
            player.isJumping = false;
        }

        if (player.isJumping) {
            player.jumpTimer += deltaTime;
        }
        else {
            player.jumpTimer = 0.0f;
        }

        vel.velocity = dirNorm * player.moveSpeed;

        camera.lookAt = tfm.pos;
        camera.pos = tfm.pos;

        if (player.isJumping) {
            int animIndex = static_cast<int>(player.Jump);
            fsm.transition_state(animIndex, true, mesh.resource, player.jumpDuration);
            continue;
        }
        else if (glm::length(dir) > 0) {
            int animIndex = static_cast<int>(player.Walk);
            fsm.transition_state(animIndex, false, mesh.resource, 1.0f);
        }
        else {
            int animIndex = static_cast<int>(player.Idle);
            EventQueue::add_event(entity, PLAYER_IDLE);
            fsm.transition_state(animIndex, false, mesh.resource, 1.0f);
            continue;
        }

        if (!glm::length(dir) > 0.0f) {
            continue;
        }

        EventQueue::add_event(entity, PLAYER_MOVING);

        vel.dirNorm = dirNorm;
    }
}
