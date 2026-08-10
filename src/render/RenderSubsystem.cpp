#include "render/RenderSubsystem.h"

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

static std::vector<uint32_t> makeCubeIndices() {
    return {
        0, 1, 2,  2, 3, 0,   // front
        5, 4, 7,  7, 6, 5,   // back
        4, 0, 3,  3, 7, 4,   // left
        1, 5, 6,  6, 2, 1,   // right
        3, 2, 6,  6, 7, 3,   // top
        4, 5, 1,  1, 0, 4    // bottom
    };
}

RenderSubsystem::RenderSubsystem(rhi::IRenderBackend& backend,
                                 float aspectRatio)
    : m_backend(backend), m_aspectRatio(aspectRatio) {
}

void RenderSubsystem::init() {
    m_shader = std::make_unique<Shader>(m_backend, VERT_SRC, FRAG_SRC);
    m_cube   = std::make_unique<Mesh>(m_backend,
                                      makeCubeVertices(),
                                      makeCubeIndices());
    m_camera = std::make_unique<Camera>(
        glm::vec3(0.0f, 0.0f, 3.0f), m_aspectRatio);

    // Was glEnable(GL_DEPTH_TEST) at Step 6 — now expressed in the
    // engine's vocabulary, not OpenGL's.
    m_backend.setDepthTest(true);

    std::cout << "  -> cube: " << m_cube->getIndexCount()
              << " indices, depth test enabled" << std::endl;
}

void RenderSubsystem::update(float dt) {
    m_cubeTransform.rotation.y += 45.0f * dt;
    m_cubeTransform.rotation.x += 20.0f * dt;

    m_shader->use();
    m_shader->setMat4("uModel",      m_cubeTransform.getModelMatrix());
    m_shader->setMat4("uView",       m_camera->getViewMatrix());
    m_shader->setMat4("uProjection", m_camera->getProjectionMatrix());

    m_cube->draw();
}

void RenderSubsystem::shutdown() {
    m_cube.reset();
    m_shader.reset();
    m_camera.reset();
}
