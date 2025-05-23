
#include <entt/entt.hpp>
#include "glmcommon.hpp"
#include <glm/gtx/quaternion.hpp> //..
#include "imgui.h"
#include "Log.hpp"
#include "Game.hpp"
#include "GameComponents.hpp"
#include "EngineComponents.hpp"
#include "FSM.h"
#include "Subject.h"
#include "TestObserver.h"
#include "EventQueue.h"
#include "PhysicsObserver.h"
#include "QuestObserver.h"

std::vector<Sphere*> testSpheres();

bool Game::init()
{
    srand(time(0));

    forwardRenderer = std::make_shared<eeng::ForwardRenderer>();
    forwardRenderer->init("shaders/phong_vert.glsl", "shaders/phong_frag.glsl");

    shapeRenderer = std::make_shared<ShapeRendering::ShapeRenderer>();
    shapeRenderer->init();
    
    entity_registry = std::make_shared<entt::registry>();

    Plane plane{ glm::vec3(0,0,1), 50.0f };
    auto wall1 = entity_registry->create();
    entity_registry->emplace<Plane>(wall1, plane);

    auto AABBTest = entity_registry->create();

    SphereColliderComponent sphereCol{};
    sphereCol.sphereCollider = Sphere{ glm::vec3(0, 2, 0), 1.0f, glm::vec3(0, 2, 0) };
    sphereCol.isTrigger = false;
    sphereCol.sphereCollider.entityOwner = AABBTest;

    AABBColliderComponent aabbCol{};
    aabbCol.AABBCollider.localPos = glm::vec3(0, 2, 0);
    aabbCol.AABBCollider.center = glm::vec3(0, 2, 0);
    aabbCol.AABBCollider.halfWidths[0] = 0.5f;
    aabbCol.AABBCollider.halfWidths[1] = 2.0f;
    aabbCol.AABBCollider.halfWidths[2] = 0.5f;
    aabbCol.isTrigger = false;
    aabbCol.entityOwner = AABBTest;

    TransformComponent tfm{ glm::vec3(10.0f, 0.0f, 10.0f),
                        glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),  
                        glm::vec3(0.03f) };                                               

    entity_registry->emplace<AABBColliderComponent>(AABBTest, aabbCol);
    entity_registry->emplace<SphereColliderComponent>(AABBTest, sphereCol);
    entity_registry->emplace<TransformComponent>(AABBTest, tfm);


    instanceCreator = InstanceCreator(entity_registry);
    fsm = FSM();
    fsmTest = FSM();

    moveSys = MovementSystem(entity_registry);
    fsmSys = FSMSystem(entity_registry);
    npcSys = NPCSystem(entity_registry);
    renderSys = RenderSystem(entity_registry, forwardRenderer);
    playerSys = PlayerSystem(entity_registry);
    physSys = PhysicsSystem(entity_registry);

    BVHRoot = bvh.BuildBVHBottomUp(testSpheres(), 10.0f);
    instanceCreator.create_type(InstanceCreator::INTANCE_TYPE::PLAYER);

    auto view = entity_registry->view<PlayerControllerComponent>();
    Observer* questObserver;
    entt::entity playerId;
    for (auto [entity, pcc] : view.each()) {
        questObserver = new QuestObserver(entity);
        playerId = entity;
    }
    
    Observer* test = new TestObserver();
    Observer* physicsObserver = new PhysicsObserver();
    Subject::init();
    Subject::add_observer(test);
    Subject::add_observer(physicsObserver);
    Subject::add_observer(questObserver);
    EventQueue::init();
    
    EventQueue::add_event(playerId, PICK_UP_FOOD);

    instanceCreator.create_type(InstanceCreator::INTANCE_TYPE::GRASS, glm_aux::vec3_000);
    instanceCreator.create_type(InstanceCreator::INTANCE_TYPE::NPC);

    return true;
}

std::vector<Sphere*> testSpheres()
{
    std::vector<Sphere*> spheres;
    for (int i = 0; i < 40; ++i) {
        float x = static_cast<float>(std::rand() % 301) / 10.0f; // 0 to 30
        float z = static_cast<float>(std::rand() % 301) / 10.0f; // 0 to 30
        glm::vec3 pos(x, 1.0f, z);
        Sphere* s = new Sphere{ pos, 1.0f, pos };
        spheres.push_back(s);
    }
    return spheres;
}

void Game::update(
    float time,
    float deltaTime,
    InputManagerPtr input)
{
    //player_controller_system(input, deltaTime);
    playerSys.update(input, deltaTime, camera, fsm);

    //auto view = entity_registry->view<PlayerControllerComponent, SphereColliderComponent>();

    //for (auto [entity, pcc, sphere] : view.each()) {
    //    std::vector<Sphere*> possibleIntersections = bvh.FindPossibleCollisions(BVHRoot, &sphere.sphereCollider);

    //    for (auto possibleSphere : possibleIntersections) {
    //        shapeRenderer->push_states(ShapeRendering::Color4u::Purple);
    //        shapeRenderer->push_states(glm_aux::TS(possibleSphere->center, glm::vec3(1.0f, 1.0f, 1.0f)));
    //        shapeRenderer->push_sphere_wireframe(2.0f, possibleSphere->radius);
    //        shapeRenderer->pop_states<ShapeRendering::Color4u, glm::mat4>();
    //    }
    //}
    ImGui::Begin("Control colliders");

    auto controlView = entity_registry->view<SphereColliderComponent, AABBColliderComponent>();
    for (auto [entity, sphere, AABB] : controlView.each()) {
        ImGui::Text("sphere : isToggle %i for entity: %i", sphere.isTrigger, entity);
        ImGui::Text("aabb : isToggle %i for entity: %i", AABB.isTrigger, entity);

        if (ImGui::Button("Toggle isTrigger")) {
            sphere.isTrigger = !sphere.isTrigger;
            AABB.isTrigger = !AABB.isTrigger;
        }
    }

    ImGui::End();

    updateCamera(input);
    //NPC_controller_system();
    npcSys.update();
    //movement_system(deltaTime);
    moveSys.update(deltaTime);

    auto beforeView = entity_registry->view<TransformComponent, SphereColliderComponent, AABBColliderComponent>();

    for (auto [entity, tfm, sphere, aabb] : beforeView.each()) {
        ImGui::Begin("before phys");
        ImGui::Text("%f %f %f", tfm.pos.x, tfm.pos.y, tfm.pos.z);
        ImGui::End();
    }
    physSys.update(shapeRenderer);

    auto afterView = entity_registry->view<TransformComponent, SphereColliderComponent, AABBColliderComponent>();

    for (auto [entity, tfm, sphere, aabb] : afterView.each()) {
        ImGui::Begin("after phys");
        ImGui::Text("%f %f %f", tfm.pos.x, tfm.pos.y, tfm.pos.z);
        ImGui::End();
    }

    fsm.tick(deltaTime, time);
    fsmTest.tick(deltaTime, time);

    //FSM_system(deltaTime, time);
    fsmSys.update(deltaTime, time, fsm);


    pointlight.pos = glm::vec3(
        glm_aux::R(time * 0.1f, { 0.0f, 1.0f, 0.0f }) *
        glm::vec4(100.0f, 100.0f, 100.0f, 1.0f));

    EventQueue::process_events();
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

    //render_system(time);
    renderSys.update(time);

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


