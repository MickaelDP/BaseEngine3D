#pragma once

#include "rhi/IRenderBackend.h"

#include <glm/glm.hpp>
#include <vector>

// Step 7: Mesh is now backend-agnostic.
//
// Compare with the Step 4 version: it included glad, held GLuint members
// and called glGenVertexArrays directly. It now holds opaque handles and
// talks only to IRenderBackend. Nothing in this file would change when a
// VulkanBackend is added.
struct Vertex {
    glm::vec3 position;
};

class Mesh {
    public:
        Mesh(rhi::IRenderBackend& backend,
             const std::vector<Vertex>& vertices,
             const std::vector<uint32_t>& indices);

        // Non-indexed variant.
        Mesh(rhi::IRenderBackend& backend,
             const std::vector<Vertex>& vertices);

        ~Mesh();

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        void draw() const;

        uint32_t getVertexCount() const { return m_vertexCount; }
        uint32_t getIndexCount()  const { return m_indexCount; }

    private:
        // Reference rather than a copy: the backend outlives every Mesh,
        // and there must only ever be one instance of it.
        rhi::IRenderBackend& m_backend;

        rhi::BufferHandle      m_vertexBuffer;
        rhi::BufferHandle      m_indexBuffer;
        rhi::VertexArrayHandle m_vao;

        uint32_t m_vertexCount = 0;
        uint32_t m_indexCount  = 0;

        void createFrom(const std::vector<Vertex>& vertices,
                        const std::vector<uint32_t>* indices);
};
