#include "core/Window.hpp"
#include <iostream>

int main() {
    Window window(800, 600, "Minecraft.cpp");

    // Main loop
    while (window.isOpen()) {
        if (window.isKeyPressed(GLFW_KEY_ESCAPE)) {
            window.close();
        }
        
        window.update();
        window.swapBuffers();
    }

    return 0;
}
