#pragma once
#include "glmcommon.hpp"
#include <RenderableMesh.hpp>

struct TransformComponent {
    glm::vec3 pos;
    glm::quat rot;
    glm::vec3 scale;

    glm::mat4 worldMatrix;
};

struct MeshComponent {
    std::shared_ptr<eeng::RenderableMesh> resource;
};

