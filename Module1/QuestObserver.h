#pragma once
#include "Observer.h"

class QuestObserver : public Observer
{
public:
	entt::entity playerEntityId;
	QuestObserver(entt::entity player) { playerEntityId = player; };
	void onNotify(const entt::entity& entity, Event event) override;
	const char* questText = "none";
	int questStep = 0;
};

