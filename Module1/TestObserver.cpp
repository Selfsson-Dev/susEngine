#include "TestObserver.h"
#include "imgui.h"

void TestObserver::onNotify(const entt::entity& entity, Event event)
{
	if (event == PLAYER_MOVING) {
		ImGui::Begin("MOVE");
		ImGui::Text("PLAYER MOVED!");
		ImGui::End();
	}

	if (event == PLAYER_IDLE) {
		ImGui::Begin("IDLE");
		ImGui::Text("PLAYER IDLE!");
		ImGui::End();
	}
}
