#include "core/Window.h"
#include "core/Time.h"
#include "core/Shader.h"
#include "core/Mesh.h"
#include "scene/Camera.h"
#include "scene/Transform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

// Step 5: real 3D.
// Three changes make this the structural break from a 2D engine:
//   1. MVP matrices (Model * View * Projection) replace raw NDC coords
//   2. GL_DEPTH_TEST decides which face is in front — meaningless in 2D
//   3. dt finally drives something visible (the cube rotates)
static const char* VERT_SRC = R"(
    #version 430 core

    layout(location = 0) in vec3 aPos;

    // Sent from C++ via Shader::setMat4 once per frame.
    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;

    out vec3 vLocalPos;

    void main() {
        // Multiplication order is right-to-left: the vertex is first placed
        // in world space (Model), then relative to the camera (View), then
        // flattened onto the screen (Projection). Swapping any two produces
        // a scene that is wrong in a way that's very hard to debug visually.
        gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);

        // Passed to the fragment shader to colour faces by position, so the
        // cube's 3D structure is readable without lighting.
        vLocalPos = aPos;
    }
)";

static const char* FRAG_SRC = R"(
    #version 430 core

    in vec3 vLocalPos;
    out vec4 FragColor;

    void main() {
        // Map -0.5..0.5 local coords to 0..1 RGB. Cheap way to make the
        // cube's faces visually distinct before real lighting exists.
        FragColor = vec4(vLocalPos + 0.5, 1.0);
    }
)";

// Cube: 8 vertices, 12 triangles (2 per face), 36 indices.
// Indexed drawing pays off here — without an EBO this would need 36
// duplicated vertices instead of 8.
static std::vector<Vertex> makeCubeVertices() {
    return {
        {{ -0.5f, -0.5f,  0.5f }},  // 0 front bottom left
        {{  0.5f, -0.5f,  0.5f }},  // 1 front bottom right
        {{  0.5f,  0.5f,  0.5f }},  // 2 front top right
        {{ -0.5f,  0.5f,  0.5f }},  // 3 front top left
        {{ -0.5f, -0.5f, -0.5f }},  // 4 back bottom left
        {{  0.5f, -0.5f, -0.5f }},  // 5 back bottom right
        {{  0.5f,  0.5f, -0.5f }},  // 6 back top right
        {{ -0.5f,  0.5f, -0.5f }}   // 7 back top left
    };
}


static std::vector<GLuint> makeCubeIndices() {
    return {
        0, 1, 2,  2, 3, 0,   // front
        5, 4, 7,  7, 6, 5,   // back
        4, 0, 3,  3, 7, 4,   // left
        1, 5, 6,  6, 2, 1,   // right
        3, 2, 6,  6, 7, 3,   // top
        4, 5, 1,  1, 0, 4    // bottom
    };
}

int main() {
    try
    {
        Window window(800, 600, "BaseEngine3D - Step 1");
        Time time;
        Shader shader(VERT_SRC, FRAG_SRC);

        Mesh cube(makeCubeVertices(), makeCubeIndices());

        // Camera pulled back on +Z so the cube at the origin is in view.
        const float aspect = static_cast<float>(window.getWidth())
                           / static_cast<float>(window.getHeight());
        Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), aspect);

        Transform cubeTransform;

        // THE line that makes this 3D rather than 2D. Without it, faces
        // are drawn in index order and the back of the cube overwrites
        // the front — the shape looks inside-out and flickers.
        glEnable(GL_DEPTH_TEST);

        std::cout << "Cube ready: " << cube.getIndexCount()
                  << " indices, depth test enabled" << std::endl;

        while (!window.shouldClose())
        {
            time.update();
            const float dt = time.getDeltaTime();

            window.pollEvents();

            // --- Update ---
            // 45 degrees per SECOND, not per frame. This is the payoff of
            // Step 2: identical rotation speed at 30 or 1400 FPS.
            cubeTransform.rotation.y += 45.0f * dt;
            cubeTransform.rotation.x += 20.0f * dt;

            // --- Render ---
            glClearColor(0.1f, 0.12f, 0.18f, 1.0f);
            // The depth buffer must be cleared too, not just the colour.
            // Forgetting GL_DEPTH_BUFFER_BIT leaves last frame's depth
            // values around and geometry disappears after a few frames.
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


            shader.use();
            shader.setMat4("uModel",      cubeTransform.getModelMatrix());
            shader.setMat4("uView",       camera.getViewMatrix());
            shader.setMat4("uProjection", camera.getProjectionMatrix());

            cube.draw();
            
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