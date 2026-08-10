#include "core/Mesh.h"

void Mesh::createFrom(const std::vector<Vertex>& vertices,
                      const std::vector<uint32_t>* indices) {
    m_vertexCount = static_cast<uint32_t>(vertices.size());

    m_vertexBuffer = m_backend.createBuffer(
        rhi::BufferType::Vertex,
        vertices.data(),
        vertices.size() * sizeof(Vertex),
        rhi::BufferUsage::Static
    );

    if (indices != nullptr) {
        m_indexCount = static_cast<uint32_t>(indices->size());
        m_indexBuffer = m_backend.createBuffer(
            rhi::BufferType::Index,
            indices->data(),
            indices->size() * sizeof(uint32_t),
            rhi::BufferUsage::Static
        );
    }

    // The layout is declared in the engine's own vocabulary. The backend
    // translates it — this array would be identical for a Vulkan backend.
    static const rhi::VertexAttribute attributes[] = {
        { 0, rhi::AttributeFormat::Float3,
          static_cast<uint32_t>(offsetof(Vertex, position)) }
    };

    rhi::VertexLayout layout;
    layout.attributes = attributes;
    layout.count      = 1;
    layout.stride     = sizeof(Vertex);

    m_vao = m_backend.createVertexArray(m_vertexBuffer, m_indexBuffer, layout);
}

Mesh::Mesh(rhi::IRenderBackend& backend,
           const std::vector<Vertex>& vertices,
           const std::vector<uint32_t>& indices)
    : m_backend(backend) {
    createFrom(vertices, &indices);
}

Mesh::Mesh(rhi::IRenderBackend& backend,
           const std::vector<Vertex>& vertices)
    : m_backend(backend) {
    createFrom(vertices, nullptr);
}

Mesh::~Mesh() {
    // Resources are released through the backend, never through a raw
    // API call. The backend decides what "destroy" means for its API.
    m_backend.destroyVertexArray(m_vao);
    m_backend.destroyBuffer(m_vertexBuffer);
    m_backend.destroyBuffer(m_indexBuffer);
}

void Mesh::draw() const {
    if (m_indexCount > 0) {
        m_backend.drawIndexed(m_vao, m_indexCount);
    } else {
        m_backend.draw(m_vao, m_vertexCount);
    }
}
