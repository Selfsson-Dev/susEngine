#pragma once

#include "imgui.h"
#include "RenderableMesh.hpp"


class FSM
{
public:
	enum state { Tpose, Idle, Walk, Jump };

private:
	bool stateSwitched = false;
	bool clipDone = false;

	// blending
	float blendingSpeed = 1.0f;
	float m_blendFactor = 0.0f;
	bool blendDone = false;

	// time
	float currentTime = 0.0f;
	float currentAnimationTimer;
	float speedMarkiplier;

	void animate_whole_clip(state, std::shared_ptr<eeng::RenderableMesh> resource);

public:

	state m_lastState = state::Idle;
	state m_currentState = state::Idle;
	state m_futureState = state::Idle;
	void tick(float delta, float totalTime);
	void transition_state(state toState, bool playWhole, std::shared_ptr<eeng::RenderableMesh> resource, float duration = 1.0f);
	FSM() {};
	~FSM() {};
};