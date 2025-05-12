#pragma once
#include "glmcommon.hpp"
#include <vector>

struct LinearVelocityComponent {
    glm::vec3 velocity;
    glm::vec3 dirNorm;
};

struct PlayerControllerComponent {
    float moveSpeed;
    float jumpForce;
    bool isJumping;
    float jumpDuration;
    float jumpTimer;

    enum state { Tpose, Idle, Walk, Jump };
};

struct NPCControllerComponent {
    float moveSpeed;
    std::vector<glm::vec3> waypoints;
    int waypointIndex;
    glm::vec3 currentWaypoint;
};

struct AnimationComponent {
    int characterAnimIndex = -1;
    float characterAnimSpeed = 1.0f;
};

struct FSMComponent {
    float cooldown = 4.0f;
    float timer = 0.0f;
    int currentIndex = 0;
};