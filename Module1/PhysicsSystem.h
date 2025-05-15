#pragma once
#include "GameComponents.hpp"
#include "EngineComponents.hpp"
#include <entt/fwd.hpp>
#include <entt/entt.hpp>
#include "ShapeRenderer.hpp"
#include "BVH.h"

class PhysicsSystem
{
public:
    PhysicsSystem() {};
    PhysicsSystem(std::shared_ptr<entt::registry> entity_registry)
        : entity_registry(entity_registry) {
    }

    ~PhysicsSystem() {}

    void update(ShapeRendererPtr& shapeRenderer);
    void setTarget(entt::entity target);
private:
    std::shared_ptr<entt::registry> entity_registry;
    void updateColliderPos();
    void SphereSphereCollsion();
    void DebugDrawSpheres(ShapeRendererPtr& shapeRenderer);
    void DebugDrawAABB(ShapeRendererPtr& shapeRenderer);
    void SpherePlaneCollision();
    bool AABBAABBCollision(AABBColliderComponent& AABBa, AABBColliderComponent& AABBb);
    void AABBPLANECollision();
    std::vector<Sphere*> get_possible_collisions(Sphere* inSphere);
    void QuestHandler(AABBColliderComponent& AABBa, AABBColliderComponent& AABBb);

    entt::entity target;
    BVH bvh;
    // REALLY UGLY but should get the job done
    //entt::entity colliderA;
    //entt::entity colliderB;
};