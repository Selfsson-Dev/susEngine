#include "FSM.h"
#include <Log.hpp>



void FSM::tick(float delta, float totalTime)
{
	ImGui::Begin("FSM helper");
	ImGui::SliderFloat("blending speed", &blendingSpeed, 0.5f, 5.0f);
	if (stateSwitched) {
		m_blendFactor += delta * blendingSpeed;
	}
	currentTime = totalTime;

	if (!clipDone) {
		currentAnimationTimer += delta * speedMarkiplier;
	}
	ImGui::Text("current state: %i", static_cast<int>(m_currentState));
	ImGui::Text("future state: %i", static_cast<int>(m_futureState));
	ImGui::Text("last state: %i", static_cast<int>(m_lastState));
	ImGui::Text("Blend Factor: %f", m_blendFactor);
	ImGui::Text("Current Animation Timer: %f", currentAnimationTimer);
	ImGui::Text("Current Timer: %f", currentTime);

	ImGui::Text("Blending done: %i", blendDone);
	ImGui::Text("State switched: %i", stateSwitched);
	ImGui::End();
}

void FSM::animate_whole_clip(state stateToPlay, std::shared_ptr<eeng::RenderableMesh> resource)
{
	int currentAnimIndex = static_cast<int>(m_currentState);
	int futureAnimIndex = static_cast<int>(stateToPlay);
	m_blendFactor = glm::clamp(m_blendFactor, 0.0f, 1.0f);

	resource->animateBlend(currentAnimIndex, futureAnimIndex, currentTime, currentAnimationTimer, m_blendFactor);
}

void FSM::transition_state(state toState, bool playWholeClip, std::shared_ptr<eeng::RenderableMesh> resource, float duration)
{
	//int clipAnimIndex = static_cast<int>(toState);

	//resource->animate(clipAnimIndex, currentTime);

	//return;
	// only update blending if state switch is requsted
	if (m_futureState == m_currentState) {
		stateSwitched = false;
	}
	else {
		stateSwitched = true;
	}

	// decide if blend is done
	if (m_blendFactor >= 1.0f && stateSwitched) {
		blendDone = true;
	}

	// play whole clip by forcing quick blending 
	if (playWholeClip && !clipDone) {
		stateSwitched = true;
		int clipAnimIndex = static_cast<int>(toState);

		float clipTime = resource->m_animations[clipAnimIndex].duration_ticks / resource->m_animations[clipAnimIndex].tps; // seconds
		speedMarkiplier = clipTime / duration;

		if (currentAnimationTimer == 0.0f) {
			m_blendFactor = 0.0f;
		}

		if (currentAnimationTimer >= clipTime) {
			clipDone = true;
			currentAnimationTimer = 0.0f;
		}

		animate_whole_clip(toState, resource);
		return; // ignore rest
	}

	clipDone = false; // just to reset clip..

	// update future only if it is not the same as requested state
	if (m_futureState != toState) {
		m_lastState = m_currentState;
		m_futureState = toState;
		m_blendFactor = 0.0f;
		blendDone = false;
	}

	// make sure its never bigger than 1
	m_blendFactor = glm::clamp(m_blendFactor, 0.0f, 1.0f);
	
	// set current state if blending is done
	if (blendDone) {
		m_currentState = m_futureState;
		//m_blendFactor = 0.0f;
		int currentAnimIndex = static_cast<int>(m_currentState);
		resource->animate(currentAnimIndex, currentTime);
	}
	else {
		int currentAnimIndex = static_cast<int>(m_currentState);
		int futureAnimIndex = static_cast<int>(m_futureState);
		resource->animateBlend(currentAnimIndex, futureAnimIndex, currentTime, currentTime, m_blendFactor);
	}

}

/*
		if (m_futureState != m_currentState) {
			m_lastState = m_currentState;
			m_currentState = toState;
			m_blendFactor = 0.0f;
		}
*/

/*
	m_lastState = m_currentState;
	m_currentState = toState;
	int animationIndex = static_cast<int>(toState);
	currentAnimationDuration = resource->m_animations[animationIndex].duration_ticks / resource->m_animations[animationIndex].tps; // seconds
	

	if (m_blendFactor <= 0.0f || m_blendFactor >= 1.0f) {
		blendDone = true;
	}
	else {
		blendDone = false;
	}

	eeng::Log("current state %i", static_cast<int>(m_currentState));
	eeng::Log("future state %i", static_cast<int>(m_futureState));
	eeng::Log("last state %i", static_cast<int>(m_lastState));

	if (blendDone && m_futureState == m_currentState) {
		m_lastState = m_currentState;
		m_currentState = m_futureState;

		int animIndex = static_cast<int>(m_currentState);
		resource->animate(animIndex, currentTime); // done blending so just animate current
	}
	else {
		m_futureState = toState;

		int fromAnimIndex = static_cast<int>(m_lastState);
		int toAnimIndex = static_cast<int>(m_currentState);
		resource->animateBlend(fromAnimIndex, toAnimIndex, currentTime, currentTime, m_blendFactor);
	}
*/