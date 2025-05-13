#pragma once
#include <memory>
#include "GameComponents.hpp"
#include "EngineComponents.hpp"
#include <entt/fwd.hpp>
#include <entt/entt.hpp>
#include "imgui.h"
#include "FSM.h"

class FSMSystem{
public:
	void update(float delta, float time, FSM& fsmTest);
	FSMSystem() {};

	FSMSystem(std::shared_ptr<entt::registry> entity_registry) :
		entity_registry(entity_registry) {
	};
	~FSMSystem() {};


private:
	std::shared_ptr<entt::registry> entity_registry;
};
