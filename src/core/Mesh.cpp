#include "Mesh.h"

void Mesh::setupVertexBuffer(const std::vector<Vertex>& vertices) {
    m_vertexCount = static_cast<GLsizei>(vertices.size());

    // 1. Create the VAO — it records which buffers are bound and how
    //    their bytes map to shader inputs. Bind it FIRST: every buffer
    //    and attribute call below is captured by whichever VAO is bound.
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // 2. Create the VBO and upload the vertex data to GPU memory.
    //    GL_STATIC_DRAW tells the driver "written once, read many times",
    //    so it can place the buffer in the fastest VRAM region.
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
        vertices.data(),
        GL_STATIC_DRAW
    );

    // 3. Describe the memory layout to the GPU. Without this, the VBO is
    //    an opaque blob of bytes — the driver has no idea where a vertex
    //    starts or which field feeds which shader input.
    //
    //    Arguments: (location, components, type, normalize, stride, offset)
    //    - location 0  → matches `layout(location = 0)` in the vertex shader
    //    - 3 floats    → a vec3
    //    - stride      → bytes between two consecutive vertices
    //    - offset      → byte offset of this field inside Vertex
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, position))
    );
    glEnableVertexAttribArray(0);
}

Mesh::Mesh(const std::vector<Vertex>& vertices) {
    setupVertexBuffer(vertices);

    // Unbind the VAO so later GL calls can't accidentally modify it.
    // Order matters: unbind the VAO BEFORE the VBO, otherwise the VAO
    // records the unbinding as part of its state.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<GLuint>& indices) {
    setupVertexBuffer(vertices);

    m_indexCount = static_cast<GLsizei>(indices.size());

    // 4. Create the EBO — it lets several triangles reuse the same vertex
    //    instead of duplicating it. A quad needs 4 vertices + 6 indices
    //    rather than 6 duplicated vertices; on a large mesh this saves a
    //    significant amount of VRAM.
    //
    //    GL_ELEMENT_ARRAY_BUFFER binding IS part of the VAO state, unlike
    //    GL_ARRAY_BUFFER — so the EBO stays associated with this VAO
    //    automatically, no rebinding needed at draw time.
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)),
        indices.data(),
        GL_STATIC_DRAW
    );

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Note: GL_ELEMENT_ARRAY_BUFFER is deliberately NOT unbound here.
    // Unbinding it while the VAO is still bound would erase the EBO from
    // the VAO state. Unbinding the VAO above already detached it safely.
}

Mesh::~Mesh() {
    // glDelete* on handle 0 is a no-op per the GL spec, so this is safe
    // even for a Mesh whose construction never assigned m_ebo.
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void Mesh::draw() const {
    glBindVertexArray(m_vao);

    if (m_indexCount > 0) {
        // Indexed draw: reads vertex order from the EBO.
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    } else {
        // Sequential draw: vertices consumed in buffer order.
        glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    }

    glBindVertexArray(0);
}