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

private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    std::string m_title;

    void init();
};
