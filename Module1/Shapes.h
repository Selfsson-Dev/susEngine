#pragma once
#include "glmcommon.hpp"
#include <entt/fwd.hpp>


struct Sphere {
	glm::vec3 center;
	float radius;
	glm::vec3 localPos;
	entt::entity entityOwner;
};

struct AABBCenterHalfWidth {
	glm::vec3 center;
	float halfWidths[3];
	glm::vec3 localPos;
};

struct Plane {
	glm::vec3 normal;
	float distanceToOrigin;
};