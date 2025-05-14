#pragma once
#include <entt/fwd.hpp>

enum Event {
	NONE,
	PLAYER_MOVING,
	PLAYER_IDLE
};

struct EventDescription {
	entt::entity Entity;
	Event EventType = NONE;
};