#include "InstanceCreator.hpp"
#include <glm/gtc/random.hpp>

// creates a weak ptr from shared ptr
InstanceCreator::InstanceCreator(std::shared_ptr<entt::registry> _entity_registry_instance)
{
	entity_registry_instance = _entity_registry_instance;
}

void InstanceCreator::create_type(INTANCE_TYPE type, glm::vec3 pos)
{
	switch (type)
	{
	case PLAYER:
		create_player(pos);
		break;
	case HORSE:
		create_horse(pos);
		break;
	case NPC:
		create_npc(pos);
		break;
	case GRASS:
		create_grass(pos);
	default:
		break;
	}
}

void InstanceCreator::create_player(glm::vec3 spawn)
{
	TransformComponent tfm{ spawn,                                             // translation
							glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),   // rot to quat
							glm::vec3(0.03f) };                                                // scale


	LinearVelocityComponent vel{ glm::vec3(0.0f, 0.0f, -1.0f) };

	MeshComponent playerMesh{};
	playerMesh.resource = std::make_shared<eeng::RenderableMesh>();
	playerMesh.resource->load("assets/Amy/Ch46_nonPBR.fbx");
	playerMesh.resource->load("assets/Amy/idle.fbx", true);
	playerMesh.resource->load("assets/Amy/walking.fbx", true);
	playerMesh.resource->load("assets/Amy/jump.fbx", true);

	// Remove root motion
	playerMesh.resource->removeTranslationKeys("mixamorig:Hips");

	PlayerControllerComponent pcc{ 1.5f, 2.0f, 0, 1.5f, 0.0f};

	AnimationComponent animation{};
	animation.characterAnimIndex = 1;

	auto player = entity_registry_instance->create();

	SphereColliderComponent sphereCol{};
	sphereCol.sphereCollider = Sphere{ glm::vec3(0, 2, 0), 2.0f, glm::vec3(0, 2, 0) };
	sphereCol.isTrigger = false;
	sphereCol.sphereCollider.entityOwner = player;

	AABBColliderComponent aabbCol{};
	aabbCol.AABBCollider.localPos = glm::vec3(0, 2, 0);
	aabbCol.AABBCollider.halfWidths[0] = 0.5f;
	aabbCol.AABBCollider.halfWidths[1] = 2.0f;
	aabbCol.AABBCollider.halfWidths[2] = 0.5f;
	aabbCol.isTrigger = false;
	aabbCol.entityOwner = player;

	entity_registry_instance->emplace<TransformComponent>(player, tfm);
	entity_registry_instance->emplace<LinearVelocityComponent>(player, vel);
	entity_registry_instance->emplace<MeshComponent>(player, playerMesh);
	entity_registry_instance->emplace<PlayerControllerComponent>(player, pcc);
	entity_registry_instance->emplace<AnimationComponent>(player, animation);
	entity_registry_instance->emplace<SphereColliderComponent>(player, sphereCol);
	entity_registry_instance->emplace<AABBColliderComponent>(player, aabbCol);
}

void InstanceCreator::create_horse(glm::vec3 spawn)
{
	TransformComponent tfm{ spawn,																// translation
							glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),	// rot to quat
							glm::vec3(0.05f) };													// scale


	LinearVelocityComponent vel{ glm::vec3(0.0f, 0.0f, 0.0f) };

	MeshComponent mesh{};
	mesh.resource = std::make_shared<eeng::RenderableMesh>();
	mesh.resource->load("assets/Animals/Horse.fbx", false);

	auto testHorse = entity_registry_instance->create();

	entity_registry_instance->emplace<TransformComponent>(testHorse, tfm);
	entity_registry_instance->emplace<LinearVelocityComponent>(testHorse, vel);
	entity_registry_instance->emplace<MeshComponent>(testHorse, mesh);
}

void InstanceCreator::create_npc(glm::vec3 spawn)
{
	TransformComponent tfm{ spawn,																// translation
						glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),	// rot to quat
						glm::vec3(0.01f) };													// scale

	LinearVelocityComponent vel{ glm::vec3(0.0f, 0.0f, 0.0f) };

	MeshComponent mesh{};
	mesh.resource = std::make_shared<eeng::RenderableMesh>();
	mesh.resource->load("assets/Animals/Horse.fbx", false);

	AnimationComponent anim{};
	anim.characterAnimIndex = 5;

	NPCControllerComponent npcc{};
	std::vector<glm::vec3> vecs;
	for (int i = 0; i < 5; ++i) {
		vecs.push_back(glm::vec3(glm::linearRand(-50.0f, 50.0f), 0, glm::linearRand(-50.0f, 50.0f)));
	}
	npcc.waypoints = vecs;
	npcc.moveSpeed = glm::linearRand(5.0f, 10.5f);
	npcc.currentWaypoint = npcc.waypoints[0];

	auto npc = entity_registry_instance->create();

	entity_registry_instance->emplace<TransformComponent>(npc, tfm);
	entity_registry_instance->emplace<LinearVelocityComponent>(npc, vel);
	entity_registry_instance->emplace<MeshComponent>(npc, mesh);
	entity_registry_instance->emplace<AnimationComponent>(npc, anim);
	entity_registry_instance->emplace<NPCControllerComponent>(npc, npcc);
}

void InstanceCreator::create_grass(glm::vec3 spawn)
{
	TransformComponent tfm{ spawn,																// translation
							glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),	// rot to quat
							glm::vec3(100.0f) };												// scale

	glm::mat4 _matrix(1.0f);

	tfm.worldMatrix = glm::translate(_matrix, tfm.pos) * glm::mat4_cast(tfm.rot) * glm::scale(_matrix, tfm.scale);


	MeshComponent mesh{};
	mesh.resource = std::make_shared<eeng::RenderableMesh>();
	mesh.resource->load("assets/grass/grass_trees_merged2.fbx", false);

	auto grass = entity_registry_instance->create();

	entity_registry_instance->emplace<TransformComponent>(grass, tfm);
	entity_registry_instance->emplace<MeshComponent>(grass, mesh);
}
