#pragma once

#include "imgui.h"
#include "RenderableMesh.hpp"

class FSM
{
	enum state { Tpose, Idle, Walk, Jump };
	float blendingSpeed;
	float m_blendFactor;
	float currentTime;
	state m_originState;
	state m_lastState;
	state m_currentState;
	float currentAnimationDuration;
public:
	void tick(float delta);
	void transition_state(state toState, bool playWhole, std::shared_ptr<eeng::RenderableMesh> resource);
};