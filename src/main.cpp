#include "core/Renderer.hpp"
#include "core/Window.hpp"
#include "core/Camera.hpp"
#include "world/World.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <iostream>

int main() {
    Window window(800, 600, "Minecraft.cpp");
    Renderer renderer;

    // Initialize renderer with window dimensions (must be after window/context creation)
    renderer.init(800, 600);

    // Enable face culling for performance
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Sky blue background
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);

    // Setup 3D perspective
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
    renderer.setProjectionMatrix(projection);

    // Camera setup
    Camera camera(glm::vec3(8.0f, 100.0f, 8.0f)); // Start above the terrain
    renderer.setViewMatrix(camera.getViewMatrix());

    bool mouseCaptured = false;
    bool f11Pressed = false;

    // World setup
    World world(42); // Seed for terrain generation

    // Timing
    using clock = std::chrono::high_resolution_clock;
    auto lastTime = clock::now();

    // Main loop
    while (window.isOpen()) {
        // Time
        auto now = clock::now();
        float deltaTime = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        // ESC: release mouse or quit
        if (window.isKeyPressed(GLFW_KEY_ESCAPE)) {
            if (mouseCaptured) {
                mouseCaptured = false;
                window.enableRawMouse(false);
            } else {
                window.close();
            }
        }

        // Click to capture mouse
        if (glfwGetMouseButton(window.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !mouseCaptured) {
            mouseCaptured = true;
            window.enableRawMouse(true);
            window.getMouseDelta(); // discard the jump
        }

        // F11: toggle fullscreen
        if (window.isKeyDown(GLFW_KEY_F11)) {
            if (!f11Pressed) {
                f11Pressed = true;
                GLFWmonitor* monitor = glfwGetWindowMonitor(window.getGLFWwindow());
                if (monitor) {
                    glfwSetWindowMonitor(window.getGLFWwindow(), nullptr, 100, 100, 800, 600, 0);
                } else {
                    monitor = glfwGetPrimaryMonitor();
                    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                    glfwSetWindowMonitor(window.getGLFWwindow(), monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                }
            }
        } else {
            f11Pressed = false;
        }

        // Movement
        if (window.isKeyDown(GLFW_KEY_W)) camera.processKeyboard(Camera::FORWARD, deltaTime);
        if (window.isKeyDown(GLFW_KEY_S)) camera.processKeyboard(Camera::BACKWARD, deltaTime);
        if (window.isKeyDown(GLFW_KEY_A)) camera.processKeyboard(Camera::LEFT, deltaTime);
        if (window.isKeyDown(GLFW_KEY_D)) camera.processKeyboard(Camera::RIGHT, deltaTime);
        if (window.isKeyDown(GLFW_KEY_SPACE)) camera.processKeyboard(Camera::UP, deltaTime);
        if (window.isKeyDown(GLFW_KEY_LEFT_SHIFT)) camera.processKeyboard(Camera::DOWN, deltaTime);

        // Arrow keys for looking (fallback if mouse capture doesn't work)
        float lookSpeed = 100.0f * deltaTime;
        if (window.isKeyDown(GLFW_KEY_UP)) camera.processMouse(0, lookSpeed);
        if (window.isKeyDown(GLFW_KEY_DOWN)) camera.processMouse(0, -lookSpeed);
        if (window.isKeyDown(GLFW_KEY_LEFT)) camera.processMouse(-lookSpeed, 0);
        if (window.isKeyDown(GLFW_KEY_RIGHT)) camera.processMouse(lookSpeed, 0);

        // Mouse look (only when captured)
        auto [dx, dy] = window.getMouseDelta();
        if (mouseCaptured && (dx != 0.0 || dy != 0.0)) {
            camera.processMouse(static_cast<float>(dx), static_cast<float>(dy));
        }

        // Update world (load/unload chunks around camera)
        world.update(camera.getPosition(), 4);

        // Render
        renderer.clear();
        renderer.setViewMatrix(camera.getViewMatrix());
        world.render(renderer.getShaderProgram());

        window.update();
        window.swapBuffers();
    }

    return 0;
}
