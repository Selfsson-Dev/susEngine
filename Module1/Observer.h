#pragma once
#include <entt/fwd.hpp>
#include "EventTypes.h"

class Observer
{	
public:
	virtual ~Observer() {}
	virtual void onNotify(const entt::entity& entity, Event event) = 0;
};

