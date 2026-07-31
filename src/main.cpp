#include "core/Window.h"
#include "core/Time.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

// Step 2: the real game loop skeleton.
// Structure is now: input -> update(dt) -> render, with a properly
// measured delta time. Nothing moves yet (no camera until Step 5), so dt
// is only reported to the console to prove it is sane and stable.
int main() {
    try
    {
        Window window(800, 600, "BaseEngine3D - Step 1");

        // Time must be constructed AFTER Window: its constructor calls
        // glfwGetTime(), which is only meaningful once GLFW is initialized.
        Time time;

        while (!window.shouldClose())
        {
            // --- 1. Timing: exactly once, at the top of the frame ---
            time.update();
            const float dt = time.getDeltaTime();

            // --- 2. Input ---
            window.pollEvents();

            // --- 3. Update ---
            // Nothing to move yet. This is where camera/scene updates will
            // go from Step 5 onwards, all multiplied by dt.
            (void)dt; // silence the unused-variable warning until then


            // --- 4. Render ---
            glClearColor(0.1f, 0.12f, 0.18f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            window.swapBuffers();

            // --- 5. Validation report (Step 2 only) ---
            // Printing every frame would flood stdout and slow the loop
            // enough to distort the very measurement we want to check.
            if (time.hasSecondElapsed()) {
                std::cout << "FPS: " << time.getFPS()
                          << " | dt: " << dt * 1000.0f << " ms"
                          << std::endl;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}