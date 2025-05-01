#pragma once

#include "imgui.h"
#include "RenderableMesh.hpp"


class FSM
{
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

	void animate_whole_clip(int animIndex, std::shared_ptr<eeng::RenderableMesh> resource);

public:

	int m_lastState = 0;
	int m_currentState = 0;
	int m_futureState = 0;
	void tick(float delta, float totalTime);
	void transition_state(int toAnimIndex, bool playWhole, std::shared_ptr<eeng::RenderableMesh> resource, float duration = 1.0f);
	FSM() {};
	~FSM() {};
};