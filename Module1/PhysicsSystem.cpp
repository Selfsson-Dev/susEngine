#include "PhysicsSystem.h"
#include "EventQueue.h"
#include "imgui.h"

void PhysicsSystem::update(ShapeRendererPtr& shapeRenderer)
{
	auto view = entity_registry->view<SphereColliderComponent, AABBColliderComponent>();
	for (auto [entityA, sphereA, AABBa] : view.each()) {
		std::vector<Sphere*> possible_collisions = get_possible_collisions(&sphereA.sphereCollider);

		for (auto sphereB : possible_collisions) {
			if (entity_registry->all_of<AABBColliderComponent>(sphereB->entityOwner)) {
				auto& AABBb = entity_registry->get<AABBColliderComponent>(sphereB->entityOwner);
				if (AABBa.entityOwner == AABBb.entityOwner) continue;
				if (AABBAABBCollision(AABBa, AABBb)) {
					
				}
			}
			//auto& AABBb = entity_registry->get<AABBColliderComponent>(sphereB->entityOwner);
		}
	}

	updateColliderPos();


	auto beforeView = entity_registry->view<TransformComponent, SphereColliderComponent, AABBColliderComponent>();

	for (auto [entity, tfm, sphere, aabb] : beforeView.each()) {
		ImGui::Begin("under phys");
		ImGui::Text("%f %f %f", tfm.pos.x, tfm.pos.y, tfm.pos.z);
		ImGui::End();
	}

	//SphereSphereCollsion();
	//SpherePlaneCollision();
	//AABBAABBCollision();
	//AABBPLANECollision();
	DebugDrawSpheres(shapeRenderer);
	DebugDrawAABB(shapeRenderer);
}



void PhysicsSystem::updateColliderPos()
{
	auto view = entity_registry->view<TransformComponent, SphereColliderComponent>();

	for (auto [entity, tfm, sphere] : view.each()) {
		sphere.sphereCollider.center = sphere.sphereCollider.localPos + tfm.pos;
		tfm.pos = sphere.sphereCollider.center;
		tfm.pos.y = 0;
	}

	auto AABBView = entity_registry->view<TransformComponent, AABBColliderComponent>();

	for (auto [entity, tfm, aabb] : AABBView.each()) {
		aabb.AABBCollider.center = aabb.AABBCollider.localPos + tfm.pos;
		tfm.pos = aabb.AABBCollider.center;
		tfm.pos.y = 0;
	}

}

void PhysicsSystem::SphereSphereCollsion()
{
	auto view = entity_registry->view<SphereColliderComponent>();
	//auto innerView = entity_registry->view<SphereColliderComponent>();

	for (auto [entityA, sphereA] : view.each()) {
		for (auto [entityB, sphereB] : view.each()) {
			if (entityA == entityB) continue;

			glm::vec3 centerToCenter = sphereA.sphereCollider.center - sphereB.sphereCollider.center;
			float distance = glm::dot(centerToCenter, centerToCenter);

			float radiusSum = sphereA.sphereCollider.radius + sphereB.sphereCollider.radius;
			if (distance <= radiusSum * radiusSum) {
				EventQueue::add_event(entityA, COLLISION_SPHERE_SPHERE);
			}
		}
	}
}

void PhysicsSystem::DebugDrawSpheres(ShapeRendererPtr& shapeRenderer)
{
	auto view = entity_registry->view<SphereColliderComponent>();

	for (auto [entityA, sphereA] : view.each()) {
		shapeRenderer->push_states(ShapeRendering::Color4u::Purple);
		shapeRenderer->push_states(glm_aux::TS(sphereA.sphereCollider.center, glm::vec3(1.0f, 1.0f, 1.0f)));
		shapeRenderer->push_sphere_wireframe(2.0f, sphereA.sphereCollider.radius);
		shapeRenderer->pop_states<ShapeRendering::Color4u, glm::mat4>();
	}
}

void PhysicsSystem::DebugDrawAABB(ShapeRendererPtr& shapeRenderer)
{
	auto view = entity_registry->view<AABBColliderComponent>();

	for (auto [entityA, AABB] : view.each()) {
		shapeRenderer->push_states(ShapeRendering::Color4u::Cyan, glm_aux::T(glm::vec3(0,0,0)));
		//>shapeRenderer->push_AABB(glm::vec3(xpos, 0.0f, 0.0f), glm::vec3(xpos + 1.0f, 1.0f, 1.0f));
		float xMin = AABB.AABBCollider.center.x - AABB.AABBCollider.halfWidths[0];
		float yMin = AABB.AABBCollider.center.y - AABB.AABBCollider.halfWidths[1];
		float zMin = AABB.AABBCollider.center.z - AABB.AABBCollider.halfWidths[2];

		float xMax = AABB.AABBCollider.center.x + AABB.AABBCollider.halfWidths[0];
		float yMax = AABB.AABBCollider.center.y + AABB.AABBCollider.halfWidths[1];
		float zMax = AABB.AABBCollider.center.z + AABB.AABBCollider.halfWidths[2];

		shapeRenderer->push_AABB(glm::vec3(xMin, yMin, zMin), glm::vec3(xMax, yMax, zMax));
		shapeRenderer->pop_states<ShapeRendering::Color4u, glm::mat4>();
	}
}

void PhysicsSystem::SpherePlaneCollision()
{
	auto spheres = entity_registry->view<SphereColliderComponent>();
	auto planes = entity_registry->view<Plane>();

	//auto innerView = entity_registry->view<SphereColliderComponent>();

	for (auto [entityA, sphere] : spheres.each()) {
		for (auto [entityB, plane] : planes.each()) {
			if (entityA == entityB) continue;

			float distance = glm::dot(sphere.sphereCollider.center, plane.normal) - plane.distanceToOrigin;

			if (distance >= sphere.sphereCollider.radius) {
				EventQueue::add_event(entityA, COLLISION_SPHERE_PLANE);
			}
		}
	}
}

bool PhysicsSystem::AABBAABBCollision(AABBColliderComponent& AABBa, AABBColliderComponent& AABBb)
{
	float centerDiff = glm::abs(AABBa.AABBCollider.center.x - AABBb.AABBCollider.center.x);
	float compoundedWidth = AABBa.AABBCollider.halfWidths[0] + AABBb.AABBCollider.halfWidths[0];
	if (centerDiff > compoundedWidth) {
		return false;
	}
	
	centerDiff = glm::abs(AABBa.AABBCollider.center.y - AABBb.AABBCollider.center.y);
	compoundedWidth = AABBa.AABBCollider.halfWidths[1] + AABBb.AABBCollider.halfWidths[1];
	if (centerDiff > compoundedWidth) {
		return false;
	}
	
	centerDiff = glm::abs(AABBa.AABBCollider.center.z - AABBb.AABBCollider.center.z);
	compoundedWidth = AABBa.AABBCollider.halfWidths[2] + AABBb.AABBCollider.halfWidths[2];
	if (centerDiff > compoundedWidth) {
		return false;
	}

	glm::vec3 overlap(0.0f);

	// aabbA
	// x
	float AABBaXMax = AABBa.AABBCollider.center.x + AABBa.AABBCollider.halfWidths[0];
	float AABBaXMin = AABBa.AABBCollider.center.x - AABBa.AABBCollider.halfWidths[0];
	// y
	float AABBaYMax = AABBa.AABBCollider.center.y + AABBa.AABBCollider.halfWidths[1];
	float AABBaYMin = AABBa.AABBCollider.center.y - AABBa.AABBCollider.halfWidths[1];
	// z
	float AABBaZMax = AABBa.AABBCollider.center.z + AABBa.AABBCollider.halfWidths[2];
	float AABBaZMin = AABBa.AABBCollider.center.z - AABBa.AABBCollider.halfWidths[2];

	// aabbB
	// x
	float AABBbXMax = AABBb.AABBCollider.center.x + AABBb.AABBCollider.halfWidths[0];
	float AABBbXMin = AABBb.AABBCollider.center.x - AABBb.AABBCollider.halfWidths[0];
	// y	  b
	float AABbbYMax = AABBb.AABBCollider.center.y + AABBb.AABBCollider.halfWidths[1];
	float AABBbYMin = AABBb.AABBCollider.center.y - AABBb.AABBCollider.halfWidths[1];
	// z	  b
	float AABbbZMax = AABBb.AABBCollider.center.z + AABBb.AABBCollider.halfWidths[2];
	float AABBbZMin = AABBb.AABBCollider.center.z - AABBb.AABBCollider.halfWidths[2];


	if (AABBaXMax > AABBbXMin && AABBbXMax > AABBaXMin) {
		float overlapX1 = AABBaXMax - AABBbXMin;
		float overlapX2 = AABBbXMax - AABBaXMin;
		overlap.x = std::min(overlapX1, overlapX2);
	}

	if (AABBaYMax > AABBbYMin && AABbbYMax > AABBaYMin) {
		float overlapY1 = AABBaYMax - AABBbYMin;
		float overlapY2 = AABbbYMax - AABBaYMin;
		overlap.y = std::min(overlapY1, overlapY2);
	}

	if (AABBaZMax > AABBbZMin && AABbbZMax > AABBaZMin) {
		float overlapZ1 = AABBaZMax - AABBbZMin;
		float overlapZ2 = AABbbZMax - AABBaZMin;
		overlap.z = std::min(overlapZ1, overlapZ2);
	}

	if (!AABBa.isTrigger) {
		AABBa.AABBCollider.center.x -= overlap.x * 0.5f;
		AABBa.AABBCollider.center.z -= overlap.z * 0.5f;
	}
	if (!AABBb.isTrigger) {
		AABBb.AABBCollider.center.x += overlap.x * 0.5f;
		AABBb.AABBCollider.center.z += overlap.z * 0.5f;
	}

	//if (entity_registry->all_of<TransformComponent>(AABBa.entityOwner)) {
	//	auto& tfm = entity_registry->get<TransformComponent>(AABBa.entityOwner);
	//	tfm.pos.x -= overlap.x * 100.0f;
	//	tfm.pos.z -= overlap.y * 100.0f;
	//}

	//if (entity_registry->all_of<TransformComponent>(AABBb.entityOwner)) {
	//	auto& tfm = entity_registry->get<TransformComponent>(AABBb.entityOwner);
	//	tfm.pos.x += overlap.x * 100.0f;
	//	tfm.pos.z += overlap.y * 100.0f;
	//}

	EventQueue::add_event(AABBa.entityOwner, COLLISION_AABB_AABB);
	EventQueue::add_event(AABBb.entityOwner, COLLISION_AABB_AABB);
	return true;
}

//void PhysicsSystem::AABBAABBCollision()
//{
//	auto AABBa = entity_registry->view<AABBColliderComponent>();
//	auto AABBb = entity_registry->view<AABBColliderComponent>();
//
//	for (auto [entityA, AABBa] : AABBa.each()) {
//		for (auto [entityB, AABBb] : AABBb.each()) {
//			if (entityA == entityB) continue;
//
//			float centerDiff = glm::abs(AABBa.AABBCollider.center.x - AABBb.AABBCollider.center.x);
//			float compoundedWidth = AABBa.AABBCollider.halfWidths[0] + AABBb.AABBCollider.halfWidths[0];
//			if (centerDiff > compoundedWidth) {
//				return;
//			}
//
//			centerDiff = glm::abs(AABBa.AABBCollider.center.y - AABBb.AABBCollider.center.y);
//			compoundedWidth = AABBa.AABBCollider.halfWidths[1] + AABBb.AABBCollider.halfWidths[1];
//			if (centerDiff > compoundedWidth) {
//				return;
//			}
//
//			centerDiff = glm::abs(AABBa.AABBCollider.center.z - AABBb.AABBCollider.center.z);
//			compoundedWidth = AABBa.AABBCollider.halfWidths[2] + AABBb.AABBCollider.halfWidths[2];
//			if (centerDiff > compoundedWidth) {
//				return;
//			}
//
//			EventQueue::add_event(entityA, COLLISION_AABB_AABB);
//		}
//	}
//}

void PhysicsSystem::AABBPLANECollision()
{
	auto AABB = entity_registry->view<AABBColliderComponent>();
	auto Planes = entity_registry->view<Plane>();

	for (auto [entityA, AABBa] : AABB.each()) {
		for (auto [entityB, plane] : Planes.each()) {
			if (entityA == entityB) continue;

			float r = AABBa.AABBCollider.halfWidths[0] * glm::abs(-plane.normal.x) +
				AABBa.AABBCollider.halfWidths[1] * glm::abs(-plane.normal.y) +
				AABBa.AABBCollider.halfWidths[2] * glm::abs(-plane.normal.z);

			float s = glm::dot(plane.normal, AABBa.AABBCollider.center) - plane.distanceToOrigin;

			if (s >= r) {
				EventQueue::add_event(entityA, COLLISION_AABB_PLANE);
			}
		}
	}
}

std::vector<Sphere*> PhysicsSystem::get_possible_collisions(Sphere* inSphere)
{
	auto spheres = entity_registry->view<SphereColliderComponent>();
	std::vector<Sphere*> spheresPtrs;
	for (auto [entity, sphere] : spheres.each()) {
		spheresPtrs.push_back(&sphere.sphereCollider);
	}

	SphereNode* root = bvh.BuildBVHBottomUp(spheresPtrs, 10.0f);
	return bvh.FindPossibleCollisions(root, inSphere);
}

void PhysicsSystem::setTarget(entt::entity target)
{
	this->target = target;
}

void PhysicsSystem::QuestHandler(AABBColliderComponent& AABBa, AABBColliderComponent& AABBb)
{
	EventQueue::add_event(AABBa.entityOwner, FOOD_PICKED_UP);
	EventQueue::add_event(AABBb.entityOwner, FOOD_PICKED_UP);
}
