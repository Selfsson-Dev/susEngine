#pragma once
#include "glmcommon.hpp"
#include <vector>
struct Camera
{
    glm::vec3 lookAt = glm_aux::vec3_000;   // Point of interest
    glm::vec3 up = glm_aux::vec3_010;       // Local up-vector
    float distance = 15.0f;                 // Distance to point-of-interest
    float sensitivity = 0.005f;             // Mouse sensitivity
    const float nearPlane = 1.0f;           // Rendering near plane
    const float farPlane = 500.0f;          // Rendering far plane

    // Position and view angles (computed when camera is updated)
    float yaw = 0.0f;                       // Horizontal angle (radians)
    float pitch = -glm::pi<float>() / 8;    // Vertical angle (radians)
    glm::vec3 pos;                          // Camera position

    // Previous mouse position
    glm::ivec2 mouse_xy_prev{ -1, -1 };
};

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