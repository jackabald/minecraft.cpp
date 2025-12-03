#include "core/Renderer.hpp"
#include "core/Window.hpp"
#include "core/Camera.hpp"
#include "world/World.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <iostream>

int main() {
    Window window(800, 600, "Minecraft.cpp");
    Renderer renderer;

    // Initialize renderer with window dimensions (must be after window/context creation)
    renderer.init(800, 600);

    // Setup 3D perspective
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
    renderer.setProjectionMatrix(projection);

    // Camera setup
    Camera camera(glm::vec3(8.0f, 100.0f, 8.0f)); // Start above the terrain
    renderer.setViewMatrix(camera.getViewMatrix());
    window.enableRawMouse(true);

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

        // Input
        if (window.isKeyPressed(GLFW_KEY_ESCAPE)) {
            window.close();
        }
        if (window.isKeyDown(GLFW_KEY_W)) camera.processKeyboard(Camera::FORWARD, deltaTime);
        if (window.isKeyDown(GLFW_KEY_S)) camera.processKeyboard(Camera::BACKWARD, deltaTime);
        if (window.isKeyDown(GLFW_KEY_A)) camera.processKeyboard(Camera::LEFT, deltaTime);
        if (window.isKeyDown(GLFW_KEY_D)) camera.processKeyboard(Camera::RIGHT, deltaTime);
        if (window.isKeyDown(GLFW_KEY_SPACE)) camera.processKeyboard(Camera::UP, deltaTime);
        if (window.isKeyDown(GLFW_KEY_LEFT_SHIFT)) camera.processKeyboard(Camera::DOWN, deltaTime);

        // Mouse
        auto [dx, dy] = window.getMouseDelta();
        if (dx != 0.0 || dy != 0.0) camera.processMouse((float)dx, (float)dy);

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
