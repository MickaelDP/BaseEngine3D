#include "Window.h"
#include <glad/glad.h>
#include <stdexcept>
#include <iostream>

// GLFW callback: called by GLFW whenever the window is resized.
// Must update the GL viewport, otherwise rendering stays clamped to the
// old size while the window itself grows/shrinks.
static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // Update the OpenGL viewport dimensions to match the new window size
    glViewport(0, 0, width, height);
}

Window::Window(int width, int height, const std::string& title)
    : m_width(width), m_height(height) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // Request an OpenGL 4.3 Core context explicitly. Without this hint,
    // drivers may hand back a legacy/compatibility context that silently
    // lacks compute shader support (needed later for the baking pipeline).
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_handle) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_handle);
    glfwSetFramebufferSizeCallback(m_handle, framebufferSizeCallback);

    // glad must load the GL function pointers AFTER a context is current
    // (glfwMakeContextCurrent above), never before — there's nothing to
    // bind function addresses against until then.
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize glad (GL function loading)");
    }

    glViewport(0, 0, width, height);
}

Window::~Window() {
    // glfwTerminate() destroys all remaining windows and cleans up GLFW
    // state. Safe to call even if m_handle was never created successfully
    // (glfwInit failed) — the constructor would have thrown before
    // reaching this point in that case anyway.
    glfwTerminate();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_handle);
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_handle);
}

void Window::pollEvents() {
    glfwPollEvents();
}