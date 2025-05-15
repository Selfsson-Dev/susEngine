#pragma once
#include <entt/fwd.hpp>

enum Event {
	NONE,
	PLAYER_MOVING,
	PLAYER_IDLE,
	COLLISION_SPHERE_SPHERE,
	COLLISION_SPHERE_PLANE,
	COLLISION_AABB_AABB,
	COLLISION_AABB_PLANE,
	FOOD_PICKED_UP,
	PICK_UP_FOOD,
	FEED_HORSE
};

struct EventDescription {
	entt::entity Entity;
	Event EventType = NONE;
};