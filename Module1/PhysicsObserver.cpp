#include "PhysicsObserver.h"

void PhysicsObserver::onNotify(const entt::entity& entity, Event event)
{
	if (event == COLLISION_SPHERE_SPHERE) {
		ImGui::Begin("Sphere collision detected");
		ImGui::Text("SPHERES HAVE COLLIDED");
		ImGui::End();
	}

	if (event == COLLISION_SPHERE_PLANE) {
		ImGui::Begin("Plane-Sphere collision detected");
		ImGui::Text("SPHERE HAS COLLIDED WITH PLANE");
		ImGui::End();
	}

	if (event == COLLISION_AABB_AABB) {
		ImGui::Begin("AABB collision detected");
		ImGui::Text("AABB HAS COLLIDED WITH AABB");
		ImGui::End();
	}

	if (event == COLLISION_AABB_PLANE) {
		ImGui::Begin("AABB-PLANE collision detected");
		ImGui::Text("AABB HAS COLLIDED WITH A PLANE");
		ImGui::End();
	}
}
