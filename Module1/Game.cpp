
#include <entt/entt.hpp>
#include "glmcommon.hpp"
#include <glm/gtx/quaternion.hpp> //..
#include "imgui.h"
#include "Log.hpp"
#include "Game.hpp"
#include "GameComponents.hpp"
#include "EngineComponents.hpp"
#include "FSM.h"

bool Game::init()
{
    srand(time(0));

    forwardRenderer = std::make_shared<eeng::ForwardRenderer>();
    forwardRenderer->init("shaders/phong_vert.glsl", "shaders/phong_frag.glsl");

    shapeRenderer = std::make_shared<ShapeRendering::ShapeRenderer>();
    shapeRenderer->init();
    
    entity_registry = std::make_shared<entt::registry>();

    instanceCreator = InstanceCreator(entity_registry);
    fsm = FSM();
    fsmTest = FSM();

    instanceCreator.create_type(InstanceCreator::INTANCE_TYPE::PLAYER);
    instanceCreator.create_type(InstanceCreator::INTANCE_TYPE::GRASS, glm_aux::vec3_000);
    instanceCreator.create_type(InstanceCreator::INTANCE_TYPE::NPC);

    return true;
}

void Game::update(
    float time,
    float deltaTime,
    InputManagerPtr input)
{
    player_controller_system(input, deltaTime);
    updateCamera(input);
    NPC_controller_system();

    movement_system(deltaTime);
    fsm.tick(deltaTime, time);
    fsmTest.tick(deltaTime, time);

    FSM_system(deltaTime, time);
    //updatePlayer(deltaTime, input);

    pointlight.pos = glm::vec3(
        glm_aux::R(time * 0.1f, { 0.0f, 1.0f, 0.0f }) *
        glm::vec4(100.0f, 100.0f, 100.0f, 1.0f));
}

void Game::render(
    float time,
    int windowWidth,
    int windowHeight)
{
    renderUI();

    matrices.windowSize = glm::ivec2(windowWidth, windowHeight);

    // Projection matrix
    const float aspectRatio = float(windowWidth) / windowHeight;
    matrices.P = glm::perspective(glm::radians(60.0f), aspectRatio, camera.nearPlane, camera.farPlane);

    // View matrix
    matrices.V = glm::lookAt(camera.pos, camera.lookAt, camera.up);
    matrices.VP = glm_aux::create_viewport_matrix(0.0f, 0.0f, windowWidth, windowHeight, 0.0f, 1.0f);

    // Begin rendering pass
    forwardRenderer->beginPass(matrices.P, matrices.V, pointlight.pos, pointlight.color, camera.pos);

    render_system(time);

    static bool showBone = false;

    ImGui::Begin("BoneTester");
    if (ImGui::Button("Show bones")) {
        showBone = !showBone;
    }
    if (showBone) {
        BONEGIZMO();
    }
    ImGui::End();

    // End rendering pass
    drawcallCount = forwardRenderer->endPass();

    shapeRenderer->push_states(ShapeRendering::Color4u{ 0xFFE61A80 });
    shapeRenderer->push_AABB(character_aabb1.min, character_aabb1.max);
    shapeRenderer->push_AABB(character_aabb2.min, character_aabb2.max);
    shapeRenderer->push_AABB(character_aabb3.min, character_aabb3.max);
    shapeRenderer->push_AABB(horse_aabb.min, horse_aabb.max);
    shapeRenderer->push_AABB(grass_aabb.min, grass_aabb.max);
    shapeRenderer->pop_states<ShapeRendering::Color4u>();

//    // Draw shape batches
    // magically depends on above for some reason
    shapeRenderer->render(matrices.P * matrices.V);
    shapeRenderer->post_render();
}

void Game::renderUI()
{
    ImGui::Begin("Game Info");
    ImGui::Text("Drawcall count %i", drawcallCount);
    ImGui::Separator();

    auto view = entity_registry->view<TransformComponent, PlayerControllerComponent>();

    for (auto [entity, tfm, pcc] : view.each()) {
        ImGui::Text("Player pos: %f %f %f", tfm.pos.x, tfm.pos.y, tfm.pos.z);
        ImGui::SliderFloat("Player movespeed", &pcc.moveSpeed, 0.0f, 50.0f);
        //ImGui::SliderInt("Player animation state", &pcc.animationState, 0, 2); // overrided anyway by pc system
        ImGui::Text("isJumping: %i", pcc.isJumping);
        ImGui::Text("jumpTimer: %f", pcc.jumpTimer);
    }

    const char* items[] = { "NPC", "HORSE", "PLAYER", "GRASS"};
    static int item_current = 0;
    ImGui::SeparatorText("Spawner");

    ImGui::Combo("Entity type", &item_current, items, IM_ARRAYSIZE(items));
    static float vec4a[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    ImGui::InputFloat3("Spawn loc.", vec4a);
    glm::vec3 spawnLoc = glm::vec3(vec4a[0], vec4a[1], vec4a[2]);

    static int i0 = 0;
    ImGui::InputInt("amount", &i0);

    if (ImGui::Button("Spawn entity")) {
        for (int i = 0; i < i0; ++i) {
            if (item_current == 0) {
                instanceCreator.create_type(InstanceCreator::INTANCE_TYPE::NPC, spawnLoc);
            }
            else if (item_current == 1) {
                instanceCreator.create_type(InstanceCreator::INTANCE_TYPE::HORSE, spawnLoc);
            }
            else if (item_current == 2) {
                instanceCreator.create_type(InstanceCreator::INTANCE_TYPE::PLAYER, spawnLoc);
            }
            else if (item_current == 3) {
                instanceCreator.create_type(InstanceCreator::INTANCE_TYPE::GRASS, spawnLoc);
            }
        }
    }
    
    ImGui::Spacing();

    ImGui::End(); // end info window
}

void Game::destroy()
{

}

void Game::updateCamera(
    InputManagerPtr input)
{
    // Fetch mouse and compute movement since last frame
    auto mouse = input->GetMouseState();
    glm::ivec2 mouse_xy{ mouse.x, mouse.y };
    glm::ivec2 mouse_xy_diff{ 0, 0 };
    if (mouse.leftButton && camera.mouse_xy_prev.x >= 0)
        mouse_xy_diff = camera.mouse_xy_prev - mouse_xy;
    camera.mouse_xy_prev = mouse_xy;

    // Update camera rotation from mouse movement
    camera.yaw += mouse_xy_diff.x * camera.sensitivity;
    camera.pitch += mouse_xy_diff.y * camera.sensitivity;
    camera.pitch = glm::clamp(camera.pitch, -glm::radians(89.0f), 0.0f);

    // Update camera position
    const glm::vec4 rotatedPos = glm_aux::R(camera.yaw, camera.pitch) * glm::vec4(0.0f, 0.0f, camera.distance, 1.0f);
    camera.pos = camera.lookAt + glm::vec3(rotatedPos);
}

// Logic for movement based on velocity.
void Game::movement_system(float deltaTime)
{
    auto view = entity_registry->view<TransformComponent, LinearVelocityComponent>();

    for (auto [entity, tfm, vel] : view.each()) {

        glm::mat4 _matrix(1.0f);

        tfm.pos += vel.velocity * deltaTime;

        glm::vec3 modelFwd = glm::vec3(0, 0, 1);
        float d = glm::dot(modelFwd, vel.dirNorm);

        if (d < -0.9999f) {

            tfm.rot = glm::angleAxis(glm::pi<float>(), glm::vec3(0, 1, 0));
        }
        else if (d > 0.9999f) {
            tfm.rot = glm::quat(1, 0, 0, 0);
        }
        else {
            glm::vec3 axis = glm::normalize(glm::cross(modelFwd, vel.dirNorm));
            float angle = acos(d);
            tfm.rot = glm::angleAxis(angle, axis);
        }

        tfm.worldMatrix = glm::translate(_matrix, tfm.pos) * glm::mat4_cast(tfm.rot) * glm::scale(_matrix, tfm.scale);
    }
}

void Game::player_controller_system(InputManagerPtr input, float deltaTime) {
    // get input
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
            fsm.transition_state(animIndex, false, mesh.resource, 1.0f);
            continue;
        }
        if (!glm::length(dir) > 0.0f) {
            continue;
        }

        vel.dirNorm = dirNorm;
    }
}

// Logic to render all meshes with MeshComponent
void Game::render_system(float time) {
    auto animView = entity_registry->view<AnimationComponent, MeshComponent>();
    static float blendingAmount = 0.0f;
    static int animationState = 0;

    auto view = entity_registry->view<TransformComponent, MeshComponent>();

    for (auto [entity, tfm, mesh] : view.each()) {
        forwardRenderer->renderMesh(mesh.resource, tfm.worldMatrix);
    }
}

void Game::NPC_controller_system() {
    auto view = entity_registry->view<NPCControllerComponent, TransformComponent, LinearVelocityComponent>();

    for (auto [entity, npcc, tfm, vel] : view.each()) {
        if (glm::distance(npcc.currentWaypoint, tfm.pos) <= 0.25f) {
            npcc.waypointIndex += 1;

            if (npcc.waypointIndex + 1 <= npcc.waypoints.size()) {
                int tempWaypoint = npcc.waypointIndex;
                glm::vec3 waypoint = npcc.waypoints[tempWaypoint];
                eeng::Log("npc new waypoint: %f %f %f, index %i", waypoint.x, waypoint.y, waypoint.z, npcc.waypointIndex);
            }
        }

        if (npcc.waypointIndex >= npcc.waypoints.size()) {
            npcc.waypointIndex = 0;
            eeng::Log("npc went through all waypoints, looping back");
        }

        npcc.currentWaypoint = npcc.waypoints[npcc.waypointIndex];

        glm::vec3 dir = npcc.currentWaypoint - tfm.pos;

        vel.velocity = glm::normalize(dir) * npcc.moveSpeed;
        
        // only rotate if we actually move
        if (!glm::length(dir) > 0.0f) {
            continue;
        }
        glm::vec3 dirNorm = glm::normalize(dir);

        vel.dirNorm = dirNorm;
    }
}

void Game::BONEGIZMO()
{
    bool drawSkeleton = true;
    float axisLen = 25.0f;

    auto view = entity_registry->view<PlayerControllerComponent, MeshComponent, TransformComponent>();

    for(auto [entity, pcc, mesh, tfm] : view.each())

    if (drawSkeleton) {
        for (int i = 0; i < mesh.resource->boneMatrices.size(); ++i) {
            auto IBinverse = glm::inverse(mesh.resource->m_bones[i].inversebind_tfm);
            glm::mat4 global = tfm.worldMatrix * mesh.resource->boneMatrices[i] * IBinverse;
            glm::vec3 pos = glm::vec3(global[3]); 

            glm::vec3 right = glm::vec3(global[0]); // X
            glm::vec3 up = glm::vec3(global[1]); // Y
            glm::vec3 fwd = glm::vec3(global[2]); // Z

            shapeRenderer->push_states(ShapeRendering::Color4u::Red);
            shapeRenderer->push_line(pos, pos + axisLen * right);

            shapeRenderer->push_states(ShapeRendering::Color4u::Green);
            shapeRenderer->push_line(pos, pos + axisLen * up);

            shapeRenderer->push_states(ShapeRendering::Color4u::Blue);
            shapeRenderer->push_line(pos, pos + axisLen * fwd);

            shapeRenderer->pop_states<ShapeRendering::Color4u>();
            shapeRenderer->pop_states<ShapeRendering::Color4u>();
            shapeRenderer->pop_states<ShapeRendering::Color4u>();
        }
    }
}

void Game::FSM_system(float delta, float time)
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