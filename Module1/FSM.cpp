#include "FSM.h"

void FSM::tick(float delta)
{
	ImGui::Begin("FSM helper");
	ImGui::SliderFloat("blending speed", &blendingSpeed, 0.0f, 1.0f);
	m_blendFactor += delta * blendingSpeed;
	currentTime += delta;
	ImGui::End();
}

void FSM::transition_state(state toState, bool playWholeClip, std::shared_ptr<eeng::RenderableMesh> resource)
{
	m_lastState = m_currentState;
	m_currentState = toState;
	int animationIndex = static_cast<int>(toState);
	currentAnimationDuration = resource->m_animations[animationIndex].duration_ticks / resource->m_animations[animationIndex].tps; // seconds

	if (currentTime <= currentAnimationDuration) {
		resource->animate(toState, currentTime);
	}
}
