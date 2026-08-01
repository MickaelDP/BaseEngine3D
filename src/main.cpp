#include "core/Window.h"
#include "core/Time.h"
#include "core/Shader.h"
#include "core/Mesh.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

// Step 4: first real geometry on screen.
// The vertex shader now receives a per-vertex attribute instead of
// hardcoding a single position. Coordinates are raw NDC (Normalized
// Device Coordinates): the visible area is exactly -1..+1 on X and Y,
// with (0,0) at the centre of the window. No camera, no matrices yet —
// that's Step 5.
static const char* VERT_SRC = R"(
    #version 430 core

    // location = 0 must match the first argument of glVertexAttribPointer
    // in Mesh::setupVertexBuffer. This is the only contract between the C++
    // side and the GLSL side.
    layout(location = 0) in vec3 aPos;

    void main() {
        // Straight passthrough: the position is already in clip space.
        // w = 1.0 means "no perspective division", which is what we want
        // until projection matrices arrive in Step 5.
        gl_Position = vec4(aPos, 1.0);
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
        Shader shader(VERT_SRC, FRAG_SRC);

        // A triangle in NDC. Winding order is counter-clockwise, which is
        // OpenGL's default "front face" — relevant once back-face culling
        // is enabled in a later step.
        const std::vector<Vertex> vertices = {
            {{ -0.5f, -0.5f, 0.0f }},  // bottom left
            {{  0.5f, -0.5f, 0.0f }},  // bottom right
            {{  0.0f,  0.5f, 0.0f }}   // top centre
        };

        const std::vector<GLuint> indices = { 0, 1, 2 };

        // Mesh must be constructed AFTER Window: glGenVertexArrays and
        // friends require an active GL context.
        Mesh triangle(vertices, indices);

        std::cout << "Mesh ready: "
                  << triangle.getVertexCount() << " vertices, "
                  << triangle.getIndexCount()  << " indices" << std::endl;


        while (!window.shouldClose())
        {
            time.update();
            const float dt = time.getDeltaTime();
            (void)dt;

            window.pollEvents();

            glClearColor(0.1f, 0.12f, 0.18f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Bind order matters: the shader program first, then the
            // geometry. draw() binds the VAO internally.
            shader.use();
            triangle.draw();


            window.swapBuffers();

            if (time.hasSecondElapsed()) {
                std::cout << "FPS: " << time.getFPS() << std::endl;
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