#include "QuestObserver.h"
#include "imgui.h"

void QuestObserver::onNotify(const entt::entity& entity, Event event)
{
	if (entity == playerEntityId) {
		ImGui::Begin("QUEST");
		ImGui::Text(questText);
		ImGui::Text("target is player, %i", playerEntityId);
		ImGui::End();
	}

	if (entity == playerEntityId && event == PICK_UP_FOOD) {
		questStep = 0;
		questText = "PICK UP FOOD";
	}
	if (entity == playerEntityId && event == FOOD_PICKED_UP) {
		questStep = 1;
		questText = "FOOD PICKED UP";
	}

	if (entity == playerEntityId && event == FEED_HORSE) {
		questStep = 2;
		questText = "HORSE FED";
	}
}
