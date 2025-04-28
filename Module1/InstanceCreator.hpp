#pragma once

#include <entt/entt.hpp>
#include <entt/fwd.hpp>
#include "glmcommon.hpp"
#include "EngineComponents.hpp"
#include "GameComponents.hpp"

class InstanceCreator {
private:
	std::shared_ptr<entt::registry> entity_registry_instance;

	void create_player(glm::vec3 spawn);
	void create_horse(glm::vec3 spawn);
	void create_npc(glm::vec3 spawn);
	void create_grass(glm::vec3 spawn);

public:
	enum INTANCE_TYPE {
		PLAYER = 1,
		HORSE = 2,
		NPC = 3,
		GRASS = 4
	};

	InstanceCreator() {};
	InstanceCreator(std::shared_ptr<entt::registry> entity_registry_instance);
	void create_type(INTANCE_TYPE type, glm::vec3 pos = glm_aux::vec3_000);
};