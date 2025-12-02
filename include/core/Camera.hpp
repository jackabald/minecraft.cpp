#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    enum Movement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

    Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 3.0f),
           const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f,
           float pitch = 0.0f);

    glm::mat4 getViewMatrix() const;
    glm::vec3 getPosition() const { return Position; }

    void processKeyboard(Movement direction, float deltaTime);
    void processMouse(float xoffset, float yoffset, bool constrainPitch = true);

private:
    void updateCameraVectors();

    // Camera attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Euler Angles
    float Yaw;
    float Pitch;

    // Options
    float MovementSpeed;
    float MouseSensitivity;
};
