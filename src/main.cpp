#include "core/Window.hpp"

int main() {
    Window window(800, 600, "Minecraft.cpp");

    // Main loop
    while (window.isOpen()) {
        window.update();
        window.swapBuffers();
    }

    return 0;
}
