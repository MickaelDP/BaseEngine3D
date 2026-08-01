#include "core/Window.h"
#include "core/Time.h"
#include "core/Shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

// Step 3: compile and link a minimal shader pair, prove it works.
// No geometry yet (that's Step 4) — the validation here is that
// Shader construction succeeds without throwing, and that use()
// binds it without GL errors.
//
// The shaders are the simplest possible valid GLSL 4.30 pair:
// vertex does nothing, fragment outputs a solid color.
//
// Note: We use C++ raw string literals (R"(...)") to write multi-line 
// GLSL code cleanly without needing manual character escaping.
static const char* VERT_SRC = R"(
    #version 430 core
    void main() {
        gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
    }
)";

static const char* FRAG_SRC = R"(
    #version 430 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(0.2, 0.6, 1.0, 1.0);
    }
)";

int main() {
    try
    {
        Window window(800, 600, "BaseEngine3D - Step 1");
        Time time;

        // Shader must be constructed AFTER Window (GL context must exist
        // before any glCreate* call — same rule as glad initialization).
        Shader shader(VERT_SRC, FRAG_SRC);
        std::cout << "Shader compiled and linked OK (id="
                  << shader.getId() << ")" << std::endl;

        while (!window.shouldClose())
        {
            time.update();
            const float dt = time.getDeltaTime();

            window.pollEvents();

            (void)dt;

            glClearColor(0.1f, 0.12f, 0.18f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Bind the shader — no draw call yet, but binding proves
            // the program handle is valid and accepted by the driver.
            shader.use();

            window.swapBuffers();

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