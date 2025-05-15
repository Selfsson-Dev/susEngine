#include "BVH.h"

float BVH::DistanceBeetweenSpheres(Sphere* leftSphere, Sphere* rightSphere)
{
	float centerDistance = glm::length((rightSphere->center - leftSphere->center));
	float surfaceDistance = centerDistance - (leftSphere->radius + rightSphere->radius);

	return std::max(0.0f, surfaceDistance);
}

void BVH::FindMinMaxPoints(glm::vec3 leftCenter, glm::vec3 rightCenter, float leftRadius, float rightRadius, glm::vec3& minOut, glm::vec3& maxOut)
{
	minOut.x = std::min(leftCenter.x - leftRadius, rightCenter.x - rightRadius);
	maxOut.x = std::max(leftCenter.x + leftRadius, rightCenter.x + rightRadius);

	minOut.y = std::min(leftCenter.y - leftRadius, rightCenter.y - rightRadius);
	maxOut.y = std::max(leftCenter.y + leftRadius, rightCenter.y + rightRadius);

	minOut.z = std::min(leftCenter.z - leftRadius, rightCenter.z - rightRadius);
	maxOut.z = std::max(leftCenter.z + leftRadius, rightCenter.z + rightRadius);
}

SphereNode* BVH::BuildNodeFromSpheres(Sphere* leftSphere, Sphere* rightSphere)
{
	glm::vec3 minPoint, maxPoint;
	FindMinMaxPoints(leftSphere->center, rightSphere->center,
		leftSphere->radius, rightSphere->radius,
		minPoint, maxPoint);

	glm::vec3 midPoint = minPoint + (maxPoint - minPoint) * 0.5f;
	float radius = glm::length(maxPoint - minPoint) * 0.5f;

	return new SphereNode{ new Sphere{midPoint, radius}, nullptr, nullptr };
}

std::vector<std::pair<SphereNode*, SphereNode*>> BVH::FindPairs(std::vector<SphereNode*> openList, float maxDistance)
{
	std::vector<std::pair<SphereNode*, SphereNode*>> allPairs;
	std::vector<SphereNode*> availableSpheres = openList;
	while (!availableSpheres.empty()) {
		SphereNode* current = availableSpheres.back();
		availableSpheres.pop_back();
		float closestDistance = maxDistance;
		SphereNode* bestMatch = nullptr;
		int bestIndex = -1;
		for (int j = 0; j < availableSpheres.size(); ++j) {
			float distance = DistanceBeetweenSpheres(current-> collisionRepresentation,
				availableSpheres[j]->collisionRepresentation);
			if (distance < closestDistance) {
				closestDistance = distance;
				bestMatch = availableSpheres[j];
				bestIndex = j;
			}
		}
		if (bestMatch) {
			availableSpheres.erase(availableSpheres.begin() + bestIndex);
		}
		allPairs.push_back({ current , bestMatch });
	}
	return allPairs;
}

SphereNode* BVH::BuildBVHBottomUp(std::vector<Sphere*> spheres, float maxDistanceBetweenLeaves)
{
	std::vector<SphereNode*> openList;
	for (Sphere* sphere : spheres) {
		openList.push_back(BuildNodeFromSingleSphere(sphere));
	}
	while (openList.size() != 1) {
		auto pairs = FindPairs(openList, maxDistanceBetweenLeaves);
		openList.clear();
		for (auto pair : pairs) {
			if (pair.second) {
				auto node = BuildNodeFromSpheres(
					pair.first->collisionRepresentation, pair.second->collisionRepresentation
				);
				node->leftChild = pair.first;
				node->rightChild = pair.second;
				openList.push_back(node);
			}
			else {
				auto node = BuildNodeFromSingleSphere(pair.first->collisionRepresentation);
				node->leftChild = pair.first;
				openList.push_back(node);
			}
		}
		maxDistanceBetweenLeaves = std::numeric_limits<float>::max();
	}
	return openList[0];
}

std::vector<Sphere*> BVH::FindPossibleCollisions(SphereNode* treeRoot, Sphere* sphere)
{
	std::vector<Sphere*> possibleCollisions;
	if (!sphere || !treeRoot)
		return possibleCollisions;
	if (!SphereSphereCollision(treeRoot->collisionRepresentation, sphere))
		return possibleCollisions;
	if (!treeRoot-> leftChild && !treeRoot-> rightChild)
	{
		possibleCollisions.push_back(treeRoot-> collisionRepresentation);
		return possibleCollisions;
	}
	auto collisions = FindPossibleCollisions(treeRoot-> leftChild, sphere);
	possibleCollisions.insert(possibleCollisions.end(), collisions.begin(), collisions.end());
	collisions = FindPossibleCollisions(treeRoot-> rightChild, sphere);
	possibleCollisions.insert(possibleCollisions.end(), collisions.begin(), collisions.end());
	return possibleCollisions;
}

bool BVH::SphereSphereCollision(Sphere* sphereA, Sphere* sphereB)
{
	glm::vec3 centerToCenter = sphereA->center - sphereB->center;
	float distance = glm::dot(centerToCenter, centerToCenter);

	float radiusSum = sphereA->radius + sphereB->radius;
	return distance <= radiusSum * radiusSum;
}
