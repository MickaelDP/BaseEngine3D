#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

// Step 1: RAII wrapper around a GLFW window + OpenGL context.
// Single responsibility: own the GLFW window lifecycle. No rendering
// logic here, no game logic. Just: create, know when to close, swap,
// poll events, destroy.
class Window {
    public:
        Window(int width, int height, const std::string& title);
        ~Window();

        // Non-copyable: this class owns a raw GLFWwindow* handle.
        // Copying it would double-free on destruction.
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete; // The 'operator' keyword is used to redefine or overload standard C++ operators (like =, +, ==) for custom classes.

        // Check if the window has been instructed to close (e.g., user clicked the X button)
        bool shouldClose() const;
        // Swap the front and back buffers to display the rendered frame on screen
        void swapBuffers();
        // Process pending window events (keyboard, mouse input, window resizing, etc.)
        void pollEvents();

        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }

    private:
        // Raw pointer to the underlying GLFW window handle, initialized to nullptr for safety
        GLFWwindow* m_handle = nullptr;
        int m_width;
        int m_height;
};