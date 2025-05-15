#pragma once
#include "Shapes.h"
#include <vector>

struct SphereNode {
	Sphere* collisionRepresentation;
	SphereNode* leftChild;
	SphereNode* rightChild;
};

class BVH
{
public:
	float DistanceBeetweenSpheres(Sphere* leftSphere, Sphere* rightSphere);
	void FindMinMaxPoints(glm::vec3 leftCenter, glm::vec3 rightCenter, float leftRadius, float rightRadius, glm::vec3& minOut, glm::vec3& maxOut);

	SphereNode* BuildNodeFromSingleSphere(Sphere* sphere) {
		return new SphereNode{ sphere, nullptr, nullptr };
	}

	SphereNode* BuildNodeFromSpheres(Sphere* leftSphere, Sphere* rightSphere);
	std::vector<std::pair<SphereNode*, SphereNode*>>FindPairs(std::vector<SphereNode*> openList,
		float maxDistance);
	SphereNode* BuildBVHBottomUp(std::vector<Sphere*> spheres, float maxDistanceBetweenLeaves);
	std::vector<Sphere*> FindPossibleCollisions(SphereNode* treeRoot, Sphere* sphere);
	bool SphereSphereCollision(Sphere* sphereA, Sphere* sphereB);
};

