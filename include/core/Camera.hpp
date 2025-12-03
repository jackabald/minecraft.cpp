#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    enum Movement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

    // Standard camera setup with position, up vector, yaw/pitch
    Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 3.0f),
           const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f,
           float pitch = 0.0f);

    glm::mat4 getViewMatrix() const;
    glm::vec3 getPosition() const { return Position; }

    // Handle keyboard input
    void processKeyboard(Movement direction, float deltaTime);
    // Handle mouse look
    void processMouse(float xoffset, float yoffset, bool constrainPitch = true);

private:
    void updateCameraVectors();

    // Position and orientation
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Euler angles
    float Yaw;
    float Pitch;

    // Movement and look sensitivity
    float MovementSpeed;
    float MouseSensitivity;
};
