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
    
    // Input methods
    bool isKeyPressed(int key) const;
    bool isKeyDown(int key) const;
    
    // Mouse / cursor
    void enableRawMouse(bool enabled);
    // Returns mouse delta (dx, dy) since last call
    std::pair<double, double> getMouseDelta();

private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    std::string m_title;
    // Mouse tracking
    double m_lastMouseX;
    double m_lastMouseY;
    bool m_firstMouse;

    void init();
};
