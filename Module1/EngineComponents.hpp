#pragma once
#include "glmcommon.hpp"
#include <RenderableMesh.hpp>
#include "Shapes.h"

struct TransformComponent {
    glm::vec3 pos;
    glm::quat rot;
    glm::vec3 scale;

    glm::mat4 worldMatrix;
};

struct MeshComponent {
    std::shared_ptr<eeng::RenderableMesh> resource;
};

struct SphereColliderComponent {
    Sphere sphereCollider;
    bool isTrigger;
};

struct AABBColliderComponent {
    AABBCenterHalfWidth AABBCollider;
    bool isTrigger;
    entt::entity entityOwner;
};