#pragma once

#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    bool isOpen() const;
    void update();
    void swapBuffers();
    void close();
    
    // Keyboard input
    bool isKeyPressed(int key) const;
    bool isKeyDown(int key) const;
    
    // Mouse stuff
    void enableRawMouse(bool enabled);
    // Returns how much the mouse moved (dx, dy) since last call
    std::pair<double, double> getMouseDelta();

private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    std::string m_title;
    // Track mouse position between frames
    double m_lastMouseX;
    double m_lastMouseY;
    bool m_firstMouse;

    void init();
};
