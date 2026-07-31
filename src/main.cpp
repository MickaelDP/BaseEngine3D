#include "core/Window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

// Step 1: real window + real OpenGL 4.3 context + a solid clear color.
// No game loop / dt measurement yet (that's Step 2) — this loop only
// exists to keep the window open long enough to see the result.
int main() {
    try
    {
        Window window(800, 600, "BaseEngine3D - Step 1");

        while (!window.shouldClose())
        {
            window.pollEvents();

            // Dark blue-gray, arbitrary — just needs to visibly differ
            // from pure black so we know the clear actually ran.
            glClearColor(0.1f, 0.12f, 0.18f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            window.swapBuffers();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}