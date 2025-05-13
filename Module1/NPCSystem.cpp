#include "NPCSystem.hpp"

void NPCSystem::update() {
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