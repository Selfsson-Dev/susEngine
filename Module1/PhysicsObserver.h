#pragma once
#include "Observer.h"
#include "imgui.h"

class PhysicsObserver : public Observer
{
	void onNotify(const entt::entity& entity, Event event) override;
};

