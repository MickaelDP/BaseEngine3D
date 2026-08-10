#pragma once

#include "rhi/IRenderBackend.h"

#include <glad/glad.h>
#include <unordered_map>

// Step 7: the OpenGL implementation of IRenderBackend.
//
// This is now the ONLY file in the engine allowed to include glad or to
// call a gl* function. That constraint is the whole deliverable of this
// step — a grep for "gl" outside src/rhi/ should come back empty.
//
// Handles issued here happen to be the raw GL names, but nothing outside
// relies on that. A VulkanBackend would return indices into its own
// resource pools, and no call site would notice.

namespace rhi {

class OpenGLBackend : public IRenderBackend {
    public:
        OpenGLBackend() = default;
        ~OpenGLBackend() override;

        void init() override;
        void shutdown() override;
        const char* getApiName() const override { return "OpenGL 4.3 Core"; }

        BufferHandle createBuffer(BufferType type,
                                  const void* data,
                                  size_t sizeBytes,
                                  BufferUsage usage) override;
        void destroyBuffer(BufferHandle handle) override;

        VertexArrayHandle createVertexArray(
            BufferHandle vertexBuffer,
            BufferHandle indexBuffer,
            const VertexLayout& layout) override;
        void destroyVertexArray(VertexArrayHandle handle) override;

        ShaderHandle createShader(const char* vertexSrc,
                                  const char* fragmentSrc) override;
        void destroyShader(ShaderHandle handle) override;
        void bindShader(ShaderHandle handle) override;

        void setUniformMat4(ShaderHandle shader, const std::string& name,
                            const glm::mat4& value) override;
        void setUniformVec3(ShaderHandle shader, const std::string& name,
                            const glm::vec3& value) override;
        void setUniformFloat(ShaderHandle shader, const std::string& name,
                             float value) override;

        void draw(VertexArrayHandle vao, uint32_t vertexCount) override;
        void drawIndexed(VertexArrayHandle vao, uint32_t indexCount) override;

        void setViewport(int x, int y, int width, int height) override;
        void clear(const glm::vec4& color, bool clearDepth) override;
        void setDepthTest(bool enabled) override;

    private:
        // The backend must remember which GL buffer target each handle
        // belongs to: glDeleteBuffers doesn't need it, but binding during
        // VAO setup does, and the caller no longer tells us.
        std::unordered_map<uint32_t, BufferType> m_bufferTypes;

        // Uniform locations are looked up by string on every call, which
        // is a known GL performance trap. Caching them per (program, name)
        // avoids a driver round-trip every frame.
        std::unordered_map<uint64_t, GLint> m_uniformCache;

        GLint getUniformLocation(ShaderHandle shader, const std::string& name);

        static GLenum toGLTarget(BufferType type);
        static GLenum toGLUsage(BufferUsage usage);
        static GLint  formatComponentCount(AttributeFormat format);
        static GLuint compileStage(GLenum type, const char* src);
};

} // namespace rhi
