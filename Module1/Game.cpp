
#include <entt/entt.hpp>
#include "glmcommon.hpp"
#include <glm/gtx/quaternion.hpp> //..
#include "imgui.h"
#include "Log.hpp"
#include "Game.hpp"
#include "GameComponents.hpp"
#include "EngineComponents.hpp"

bool Game::init()
{
    srand(time(0));

    forwardRenderer = std::make_shared<eeng::ForwardRenderer>();
    forwardRenderer->init("shaders/phong_vert.glsl", "shaders/phong_frag.glsl");

    shapeRenderer = std::make_shared<ShapeRendering::ShapeRenderer>();
    shapeRenderer->init();
    
    entity_registry = std::make_shared<entt::registry>();

    instanceCreator = InstanceCreator(entity_registry);

    instanceCreator.create_type(InstanceCreator::INTANCE_TYPE::PLAYER);
    instanceCreator.create_type(InstanceCreator::INTANCE_TYPE::GRASS, glm_aux::vec3_000);
    instanceCreator.create_type(InstanceCreator::INTANCE_TYPE::NPC);
    //instanceCreator.create_type(InstanceCreator::INTANCE_TYPE::NPC);
    //instanceCreator.create_type(InstanceCreator::INTANCE_TYPE::NPC);

    // Character
    //characterMesh = std::make_shared<eeng::RenderableMesh>();
#if 0
    // Character
    characterMesh->load("assets/Ultimate Platformer Pack/Character/Character.fbx", false);
#endif
#if 0
    // Enemy
    characterMesh->load("assets/Ultimate Platformer Pack/Enemies/Bee.fbx", false);
#endif
#if 0
    // ExoRed 5.0.1 PACK FBX, 60fps, No keyframe reduction
    characterMesh->load("assets/ExoRed/exo_red.fbx");
    characterMesh->load("assets/ExoRed/idle (2).fbx", true);
    characterMesh->load("assets/ExoRed/walking.fbx", true);
    // Remove root motion
    characterMesh->removeTranslationKeys("mixamorig:Hips");
#endif
#if 1
    // Amy 5.0.1 PACK FBX
    //characterMesh->load("assets/Amy/Ch46_nonPBR.fbx");
    //characterMesh->load("assets/Amy/idle.fbx", true);
    //characterMesh->load("assets/Amy/walking.fbx", true);
    //// Remove root motion
    //characterMesh->removeTranslationKeys("mixamorig:Hips");
#endif
#if 0
    // Eve 5.0.1 PACK FBX
    // Fix for assimp 5.0.1 (https://github.com/assimp/assimp/issues/4486)
    // FBXConverter.cpp, line 648: 
    //      const float zero_epsilon = 1e-6f; => const float zero_epsilon = Math::getEpsilon<float>();
    characterMesh->load("assets/Eve/Eve By J.Gonzales.fbx");
    characterMesh->load("assets/Eve/idle.fbx", true);
    characterMesh->load("assets/Eve/walking.fbx", true);
    // Remove root motion
    characterMesh->removeTranslationKeys("mixamorig:Hips");
#endif

    //grassWorldMatrix = glm_aux::TRS(
    //    { 0.0f, 0.0f, 0.0f },
    //    0.0f, { 0, 1, 0 },
    //    { 100.0f, 100.0f, 100.0f });

    return true;
}

void Game::update(
    float time,
    float deltaTime,
    InputManagerPtr input)
{
    player_controller_system(input);
    updateCamera(input);
    NPC_controller_system();

    movement_system(deltaTime);

    //updatePlayer(deltaTime, input);

    pointlight.pos = glm::vec3(
        glm_aux::R(time * 0.1f, { 0.0f, 1.0f, 0.0f }) *
        glm::vec4(100.0f, 100.0f, 100.0f, 1.0f));

    // Intersect player view ray with AABBs of other objects 
    glm_aux::intersect_ray_AABB(player.viewRay, character_aabb2.min, character_aabb2.max);
    glm_aux::intersect_ray_AABB(player.viewRay, character_aabb3.min, character_aabb3.max);
    glm_aux::intersect_ray_AABB(player.viewRay, horse_aabb.min, horse_aabb.max);

    // We can also compute a ray from the current mouse position,
    // to use for object picking and such ...
    if (input->GetMouseState().rightButton)
    {
        glm::ivec2 windowPos(camera.mouse_xy_prev.x, matrices.windowSize.y - camera.mouse_xy_prev.y);
        auto ray = glm_aux::world_ray_from_window_coords(windowPos, matrices.V, matrices.P, matrices.VP);
        // Intersect with e.g. AABBs ...

        eeng::Log("Picking ray origin = %s, dir = %s",
            glm_aux::to_string(ray.origin).c_str(),
            glm_aux::to_string(ray.dir).c_str());
    }
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

    // Grass
    //forwardRenderer->renderMesh(grassMesh, grassWorldMatrix);
    //grass_aabb = grassMesh->m_model_aabb.post_transform(grassWorldMatrix);

    // Horse
    //horseMesh->animate(3, time);
    //forwardRenderer->renderMesh(horseMesh, horseWorldMatrix);
    //horse_aabb = horseMesh->m_model_aabb.post_transform(horseWorldMatrix);
    render_system(time);

    //// Character, instance 1
    //characterMesh->animate(characterAnimIndex, time * characterAnimSpeed);
    ////forwardRenderer->renderMesh(characterMesh, characterWorldMatrix1);
    //character_aabb1 = characterMesh->m_model_aabb.post_transform(characterWorldMatrix1);

    //// Character, instance 2
    //characterMesh->animate(1, time * characterAnimSpeed);
    //forwardRenderer->renderMesh(characterMesh, characterWorldMatrix2);
    //character_aabb2 = characterMesh->m_model_aabb.post_transform(characterWorldMatrix2);

    //// Character, instance 3
    //characterMesh->animate(2, time * characterAnimSpeed);
    //forwardRenderer->renderMesh(characterMesh, characterWorldMatrix3);
    //character_aabb3 = characterMesh->m_model_aabb.post_transform(characterWorldMatrix3);

    // End rendering pass
    drawcallCount = forwardRenderer->endPass();

    // Draw player view ray
    if (player.viewRay)
    {
        shapeRenderer->push_states(ShapeRendering::Color4u{ 0xff00ff00 });
        shapeRenderer->push_line(player.viewRay.origin, player.viewRay.point_of_contact());
    }
    else
    {
        shapeRenderer->push_states(ShapeRendering::Color4u{ 0xffffffff });
        shapeRenderer->push_line(player.viewRay.origin, player.viewRay.origin + player.viewRay.dir * 100.0f);
    }
    shapeRenderer->pop_states<ShapeRendering::Color4u>();

    // Draw object bases
    {
        shapeRenderer->push_basis_basic(characterWorldMatrix1, 1.0f);
        shapeRenderer->push_basis_basic(characterWorldMatrix2, 1.0f);
        shapeRenderer->push_basis_basic(characterWorldMatrix3, 1.0f);
        shapeRenderer->push_basis_basic(grassWorldMatrix, 1.0f);
        shapeRenderer->push_basis_basic(horseWorldMatrix, 1.0f);
    }

    // Draw AABBs
    {
        shapeRenderer->push_states(ShapeRendering::Color4u{ 0xFFE61A80 });
        shapeRenderer->push_AABB(character_aabb1.min, character_aabb1.max);
        shapeRenderer->push_AABB(character_aabb2.min, character_aabb2.max);
        shapeRenderer->push_AABB(character_aabb3.min, character_aabb3.max);
        shapeRenderer->push_AABB(horse_aabb.min, horse_aabb.max);
        shapeRenderer->push_AABB(grass_aabb.min, grass_aabb.max);
        shapeRenderer->pop_states<ShapeRendering::Color4u>();
    }

#if 0
    // Demo draw other shapes
    {
        shapeRenderer->push_states(glm_aux::T(glm::vec3(0.0f, 0.0f, -5.0f)));
        ShapeRendering::DemoDraw(shapeRenderer);
        shapeRenderer->pop_states<glm::mat4>();
    }
#endif

    // Draw shape batches
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
    }
    /*
            if (ImGui::Button("Button"))
    static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
    ImGui::InputFloat3("input float3", vec4a);

    const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIIIIII", "JJJJ", "KKKKKKK" };
            static int item_current = 0;
            ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));


    */
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

    //for (auto entity : view) {
    //    auto &vel = view.get< LinearVelocityComponent
    //}

    for (auto [entity, tfm, vel] : view.each()) {
        // identity
        glm::mat4 _matrix(1.0f);

        // translation
        tfm.pos += vel.velocity * deltaTime;

        // rotation is set in other systems
        //tfm.rot = glm::angleAxis(glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        
        // scale is only set in init as is now, can be updated whenever

        // update matrix
        // glm_aux::trs only accepts rotation around specific axis
        // we want to use quaternions here! make everything a mat4... and do trs manually, rip simd instruction..
        tfm.worldMatrix = glm::translate(_matrix, tfm.pos) * glm::mat4_cast(tfm.rot) * glm::scale(_matrix, tfm.scale);
    }
}

void Game::player_controller_system(InputManagerPtr input) {
    // get input
    using Key = eeng::InputManager::Key;
    bool W = input->IsKeyPressed(Key::W);
    bool A = input->IsKeyPressed(Key::A);
    bool S = input->IsKeyPressed(Key::S);
    bool D = input->IsKeyPressed(Key::D);

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
        

    auto view = entity_registry->view<TransformComponent, LinearVelocityComponent, PlayerControllerComponent, AnimationComponent>();

    // velocity = dir * speed;
    // set velocity
    // seperate into xz and y planes to get jump also
    for (auto [entity, tfm, vel, player, anim] : view.each()) {
        //eeng::Log("normalized dir is: %f %f %f" , dirNorm.x, dirNorm.y, dirNorm.z);

        vel.velocity = dirNorm * player.moveSpeed;

        camera.lookAt = tfm.pos;
        camera.pos = tfm.pos;

        // only rotate if we actually move
        if (!glm::length(dir) > 0) {
            anim.characterAnimIndex = 1;
            continue;
        }
        else {
            anim.characterAnimIndex = 2;
        }

        glm::vec3 modelFwd = glm::vec3(0, 0, 1);
        float d = glm::dot(modelFwd, dirNorm);

        if (d < -0.9999f) {
            // Opposite direction, rotate 180 degrees around some perpendicular axis
            tfm.rot = glm::angleAxis(glm::pi<float>(), glm::vec3(0, 1, 0));
        }
        else if (d > 0.9999f) {
            // Same direction, no rotation needed
            tfm.rot = glm::quat(1, 0, 0, 0);
        }
        else {
            glm::vec3 axis = glm::normalize(glm::cross(modelFwd, dirNorm));
            float angle = acos(d);
            tfm.rot = glm::angleAxis(angle, axis);
        }

    }

    // jump??

    // animation??
}

// Logic to render all meshes with MeshComponent
void Game::render_system(float time) {
    auto animView = entity_registry->view<AnimationComponent, MeshComponent>();

    for (auto [entity, animation, mesh] : animView.each()) {
        mesh.resource->animate(animation.characterAnimIndex, animation.characterAnimSpeed * time);
    }

    auto view = entity_registry->view<TransformComponent, MeshComponent>();

    for (auto [entity, tfm, mesh] : view.each()) {

        //forwardRenderer->renderMesh(horseMesh, horseWorldMatrix);
        //horse_aabb = horseMesh->m_model_aabb.post_transform(horseWorldMatrix);
        // animate da mesh!!
        // render mesh (mesh, transfrom)
        forwardRenderer->renderMesh(mesh.resource, tfm.worldMatrix);
        //horse_aabb = mesh.resource->m_model_aabb.post_transform(tfm.worldMatrix);
    }


}

void Game::NPC_controller_system() {
    // point npc towards point
    // set velocity towards point
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

        //eeng::Log("npc currentway: %f %f %f", npcc.currentWaypoint.x, npcc.currentWaypoint.y, npcc.currentWaypoint.z);

        glm::vec3 dir = npcc.currentWaypoint - tfm.pos;

        vel.velocity = glm::normalize(dir) * npcc.moveSpeed;
        
        // only rotate if we actually move
        if (!glm::length(dir) > 0.0f) {
            continue;
        }

        glm::vec3 modelFwd = glm::vec3(0, 0, 1);
        glm::vec3 dirNorm = glm::normalize(dir);
        float d = glm::dot(modelFwd, dirNorm);

        if (d < -0.9999f) {
            // Opposite direction, rotate 180 degrees around some perpendicular axis
            tfm.rot = glm::angleAxis(glm::pi<float>(), glm::vec3(0, 1, 0));
        }
        else if (d > 0.9999f) {
            // Same direction, no rotation needed
            tfm.rot = glm::quat(1, 0, 0, 0);
        }
        else {
            glm::vec3 axis = glm::normalize(glm::cross(modelFwd, dirNorm));
            float angle = acos(d);
            tfm.rot = glm::angleAxis(angle, axis);
        }
    }
}