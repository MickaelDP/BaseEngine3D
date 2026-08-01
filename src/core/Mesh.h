#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

// Step 4: geometry container.
// Single responsibility: own a VAO + VBO (+ optional EBO), and expose
// draw(). Knows nothing about shaders, cameras or materials.
//
// A vertex here is position-only. Normals and UVs come later (Step 5+),
// which is why Vertex is a struct rather than a bare glm::vec3: adding a
// field later won't change any call site.
struct Vertex {
    glm::vec3 position;
};


class Mesh {
    public:
        // Indexed draw: vertices + indices (uses an EBO).
        Mesh(const std::vector<Vertex>& vertices,
             const std::vector<GLuint>& indices);

        // Non-indexed draw: vertices only (no EBO, sequential order).
        explicit Mesh(const std::vector<Vertex>& vertices);

        ~Mesh();

        // Non-copyable: owns three GL handles.
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        // Bind the VAO and issue the draw call.
        // A Shader must already be bound (shader.use()) before calling this.
        void draw() const;

        GLsizei getVertexCount() const { return m_vertexCount; }
        GLsizei getIndexCount()  const { return m_indexCount; }

    private:
        GLuint m_vao = 0;   // Vertex Array Object: stores the attribute layout
        GLuint m_vbo = 0;   // Vertex Buffer Object: the raw vertex data
        GLuint m_ebo = 0;   // Element Buffer Object: the index data (0 if unused)

        GLsizei m_vertexCount = 0;
        GLsizei m_indexCount  = 0;

        // Shared setup for both constructors: creates the VAO/VBO and
        // declares the attribute layout.
        void setupVertexBuffer(const std::vector<Vertex>& vertices);
};