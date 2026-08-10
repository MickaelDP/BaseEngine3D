#pragma once

#include "rhi/RenderHandles.h"

#include <glm/glm.hpp>
#include <cstddef>
#include <string>

// Step 7: the abstraction that makes a Vulkan port possible.
//
// This is the decision taken at the very start of the project: build on
// OpenGL for the learning-to-friction ratio, but behind an interface, so
// that adding a VulkanBackend later means writing one new file rather
// than rewriting Camera, Mesh and RenderSubsystem.
//
// Design rule for this interface: every method signature must be
// expressible in BOTH OpenGL and Vulkan terms. Anything that leaks a
// GL-specific concept (state machine, bind points, immediate uniforms)
// would defeat the purpose. That is why resources are created and
// destroyed explicitly rather than bound globally.

namespace rhi {

class IRenderBackend {
    public:
        virtual ~IRenderBackend() = default;

        // --- Lifecycle ---
        virtual void init()     = 0;
        virtual void shutdown() = 0;

        // Human-readable API name, for logging ("OpenGL 4.3").
        virtual const char* getApiName() const = 0;

        // --- Buffers ---
        virtual BufferHandle createBuffer(BufferType type,
                                          const void* data,
                                          size_t sizeBytes,
                                          BufferUsage usage) = 0;
        virtual void destroyBuffer(BufferHandle handle) = 0;

        // --- Vertex arrays ---
        // Ties a vertex buffer (and optional index buffer) to a layout.
        // In GL this is a VAO; in Vulkan it becomes part of the pipeline
        // state object. The caller doesn't care which.
        virtual VertexArrayHandle createVertexArray(
            BufferHandle vertexBuffer,
            BufferHandle indexBuffer,       // pass {} for non-indexed
            const VertexLayout& layout) = 0;
        virtual void destroyVertexArray(VertexArrayHandle handle) = 0;

        // --- Shaders ---
        virtual ShaderHandle createShader(const char* vertexSrc,
                                          const char* fragmentSrc) = 0;
        virtual void destroyShader(ShaderHandle handle) = 0;
        virtual void bindShader(ShaderHandle handle) = 0;

        // --- Uniforms ---
        // Named uniforms are a GL-ism; Vulkan uses uniform buffers. A
        // VulkanBackend would implement these by writing into a UBO at a
        // known offset, keeping the call site unchanged.
        virtual void setUniformMat4(ShaderHandle shader,
                                    const std::string& name,
                                    const glm::mat4& value) = 0;
        virtual void setUniformVec3(ShaderHandle shader,
                                    const std::string& name,
                                    const glm::vec3& value) = 0;
        virtual void setUniformFloat(ShaderHandle shader,
                                     const std::string& name,
                                     float value) = 0;

        // --- Draw ---
        virtual void draw(VertexArrayHandle vao, uint32_t vertexCount) = 0;
        virtual void drawIndexed(VertexArrayHandle vao, uint32_t indexCount) = 0;

        // --- Frame / state ---
        virtual void setViewport(int x, int y, int width, int height) = 0;
        virtual void clear(const glm::vec4& color, bool clearDepth) = 0;
        virtual void setDepthTest(bool enabled) = 0;
};

} // namespace rhi
