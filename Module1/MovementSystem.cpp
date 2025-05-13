#include "MovementSystem.hpp"

void MovementSystem::update(float deltaTime) {
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
