#pragma once
#include "Observer.h"

class TestObserver : public Observer
{
public:
	 void onNotify(const entt::entity& entity, Event event) override;
};

