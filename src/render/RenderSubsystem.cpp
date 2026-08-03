#include "render/RenderSubsystem.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

static const char* VERT_SRC = R"(
    #version 430 core

    layout(location = 0) in vec3 aPos;

    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;

    out vec3 vLocalPos;

    void main() {
        gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
        vLocalPos = aPos;
    }
)";

static const char* FRAG_SRC = R"(
    #version 430 core

    in vec3 vLocalPos;
    out vec4 FragColor;

    void main() {
        FragColor = vec4(vLocalPos + 0.5, 1.0);
    }
)";

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

RenderSubsystem::RenderSubsystem(float aspectRatio)
    : m_aspectRatio(aspectRatio) {
    // Deliberately empty of GL calls: no context is guaranteed to exist
    // when a subsystem is constructed. All GL work happens in init().
}

void RenderSubsystem::init() {
    // Called by the Engine once the GL context is active.
    m_shader = std::make_unique<Shader>(VERT_SRC, FRAG_SRC);
    m_cube   = std::make_unique<Mesh>(makeCubeVertices(), makeCubeIndices());
    m_camera = std::make_unique<Camera>(
        glm::vec3(0.0f, 0.0f, 3.0f), m_aspectRatio
    );

    // Depth testing is a render concern, so it belongs here rather than
    // in the Engine — a headless engine running only a compute subsystem
    // would have no reason to enable it.
    glEnable(GL_DEPTH_TEST);

    std::cout << "  -> cube: " << m_cube->getIndexCount()
              << " indices, depth test enabled" << std::endl;
}

void RenderSubsystem::update(float dt) {
    // Degrees per SECOND. The Engine hands us dt; we never touch a
    // frame counter.
    m_cubeTransform.rotation.y += 45.0f * dt;
    m_cubeTransform.rotation.x += 20.0f * dt;

    // No glClear here — the Engine owns frame boundaries. A subsystem
    // that cleared the buffer would erase whatever ran before it.
    m_shader->use();
    m_shader->setMat4("uModel",      m_cubeTransform.getModelMatrix());
    m_shader->setMat4("uView",       m_camera->getViewMatrix());
    m_shader->setMat4("uProjection", m_camera->getProjectionMatrix());

    m_cube->draw();
}

void RenderSubsystem::shutdown() {
    // Explicit reset rather than relying on the destructor: this
    // guarantees the GL handles are released while the context is still
    // alive. Letting unique_ptr free them later, after the Window has
    // been destroyed, would call glDelete* on a dead context.
    m_cube.reset();
    m_shader.reset();
    m_camera.reset();
}
