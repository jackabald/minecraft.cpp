#include "core/Window.hpp"
#include <iostream>

Window::Window(int width, int height, const std::string& title)
    : m_window(nullptr), m_width(width), m_height(height), m_title(title) {
    m_firstMouse = true;
    m_lastMouseX = width / 2.0;
    m_lastMouseY = height / 2.0;
    init();
}

Window::~Window() {
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
}

void Window::init() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    // Create a windowed mode window and its OpenGL context
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    // Make the window's context current
    glfwMakeContextCurrent(m_window);
}

bool Window::isOpen() const {
    return m_window && !glfwWindowShouldClose(m_window);
}

void Window::update() {
    // Only poll for and process events here. Clearing the frame buffer
    // should be done by the renderer so we don't erase rendered content
    // after drawing.
    glfwPollEvents();
}

void Window::swapBuffers() {
    if (m_window) {
        glfwSwapBuffers(m_window);
    }
}

void Window::close() {
    if (m_window) {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }
}

bool Window::isKeyPressed(int key) const {
    if (!m_window) return false;
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

bool Window::isKeyDown(int key) const {
    if (!m_window) return false;
    return glfwGetKey(m_window, key) == GLFW_PRESS || glfwGetKey(m_window, key) == GLFW_REPEAT;
}

void Window::enableRawMouse(bool enabled) {
    if (!m_window) return;
    glfwSetInputMode(m_window, GLFW_CURSOR, enabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    m_firstMouse = true; // reset so next delta is zero
}

std::pair<double, double> Window::getMouseDelta() {
    if (!m_window) return {0.0, 0.0};
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);
    if (m_firstMouse) {
        m_lastMouseX = xpos;
        m_lastMouseY = ypos;
        m_firstMouse = false;
        return {0.0, 0.0};
    }
    double dx = xpos - m_lastMouseX;
    double dy = m_lastMouseY - ypos; // invert Y
    m_lastMouseX = xpos;
    m_lastMouseY = ypos;
    return {dx, dy};
}
