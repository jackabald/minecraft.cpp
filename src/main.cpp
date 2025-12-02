#include "core/Renderer.hpp"
#include "core/Window.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

int main() {
    Window window(800, 600, "Minecraft.cpp");
    Renderer renderer;

    // Initialize renderer with window dimensions
    renderer.init(800, 600);

    // Setup 3D perspective
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    renderer.setProjectionMatrix(projection);

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
    renderer.setViewMatrix(view);

    // Main loop
    while (window.isOpen()) {
        if (window.isKeyPressed(GLFW_KEY_ESCAPE)) {
            window.close();
        }

        renderer.clear();
        renderer.drawCube(glm::vec3(0.0f, 0.0f, 0.0f));

        window.update();
        window.swapBuffers();
    }

    return 0;
}
