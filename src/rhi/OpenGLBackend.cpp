#include "rhi/OpenGLBackend.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace rhi {

// --- Enum translation --------------------------------------------------
// These three functions are the entire "translation layer" between the
// engine's vocabulary and OpenGL's. A Vulkan backend would have the same
// three functions returning VkBufferUsageFlags and VkFormat instead.

GLenum OpenGLBackend::toGLTarget(BufferType type) {
    switch (type) {
        case BufferType::Vertex:  return GL_ARRAY_BUFFER;
        case BufferType::Index:   return GL_ELEMENT_ARRAY_BUFFER;
        case BufferType::Storage: return GL_SHADER_STORAGE_BUFFER;
    }
    return GL_ARRAY_BUFFER;
}

GLenum OpenGLBackend::toGLUsage(BufferUsage usage) {
    switch (usage) {
        case BufferUsage::Static:  return GL_STATIC_DRAW;
        case BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
    }
    return GL_STATIC_DRAW;
}

GLint OpenGLBackend::formatComponentCount(AttributeFormat format) {
    switch (format) {
        case AttributeFormat::Float2: return 2;
        case AttributeFormat::Float3: return 3;
        case AttributeFormat::Float4: return 4;
    }
    return 3;
}

// --- Lifecycle ---------------------------------------------------------

OpenGLBackend::~OpenGLBackend() = default;

void OpenGLBackend::init() {
    // The GL context is created by Window; nothing to do here for GL.
    // A VulkanBackend would create its instance, device and swapchain at
    // this point — which is exactly why the method exists in the
    // interface even though it is nearly empty here.
    std::cout << "[Backend] " << getApiName() << " ready" << std::endl;
    std::cout << "[Backend] vendor: "
              << reinterpret_cast<const char*>(glGetString(GL_VENDOR))
              << std::endl;
}

void OpenGLBackend::shutdown() {
    m_bufferTypes.clear();
    m_uniformCache.clear();
}

// --- Buffers -----------------------------------------------------------

BufferHandle OpenGLBackend::createBuffer(BufferType type,
                                         const void* data,
                                         size_t sizeBytes,
                                         BufferUsage usage) {
    GLuint id = 0;
    glGenBuffers(1, &id);

    const GLenum target = toGLTarget(type);
    glBindBuffer(target, id);
    glBufferData(target,
                 static_cast<GLsizeiptr>(sizeBytes),
                 data,
                 toGLUsage(usage));
    glBindBuffer(target, 0);

    m_bufferTypes[id] = type;
    return BufferHandle{ id };
}

void OpenGLBackend::destroyBuffer(BufferHandle handle) {
    if (!handle.isValid()) return;
    glDeleteBuffers(1, &handle.id);
    m_bufferTypes.erase(handle.id);
}

// --- Vertex arrays -----------------------------------------------------

VertexArrayHandle OpenGLBackend::createVertexArray(
        BufferHandle vertexBuffer,
        BufferHandle indexBuffer,
        const VertexLayout& layout) {

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.id);

    // Walk the backend-neutral layout description and translate each
    // attribute into a glVertexAttribPointer call.
    for (uint32_t i = 0; i < layout.count; ++i) {
        const VertexAttribute& attr = layout.attributes[i];
        glVertexAttribPointer(
            attr.location,
            formatComponentCount(attr.format),
            GL_FLOAT,
            GL_FALSE,
            static_cast<GLsizei>(layout.stride),
            reinterpret_cast<void*>(static_cast<uintptr_t>(attr.offset))
        );
        glEnableVertexAttribArray(attr.location);
    }

    // The index buffer binding is part of VAO state in GL, so binding it
    // here is enough — no rebinding needed at draw time.
    if (indexBuffer.isValid()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.id);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return VertexArrayHandle{ vao };
}

void OpenGLBackend::destroyVertexArray(VertexArrayHandle handle) {
    if (!handle.isValid()) return;
    glDeleteVertexArrays(1, &handle.id);
}

// --- Shaders -----------------------------------------------------------

GLuint OpenGLBackend::compileStage(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetShaderInfoLog(shader, logLen, nullptr, log.data());
        glDeleteShader(shader);
        throw std::runtime_error(
            std::string("Shader compilation failed:\n") + log.data());
    }
    return shader;
}

ShaderHandle OpenGLBackend::createShader(const char* vertexSrc,
                                         const char* fragmentSrc) {
    GLuint vert = compileStage(GL_VERTEX_SHADER,   vertexSrc);
    GLuint frag = compileStage(GL_FRAGMENT_SHADER, fragmentSrc);

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    glDeleteShader(vert);
    glDeleteShader(frag);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetProgramInfoLog(program, logLen, nullptr, log.data());
        glDeleteProgram(program);
        throw std::runtime_error(
            std::string("Shader linking failed:\n") + log.data());
    }

    return ShaderHandle{ program };
}

void OpenGLBackend::destroyShader(ShaderHandle handle) {
    if (!handle.isValid()) return;
    glDeleteProgram(handle.id);
}

void OpenGLBackend::bindShader(ShaderHandle handle) {
    glUseProgram(handle.id);
}

// --- Uniforms ----------------------------------------------------------

GLint OpenGLBackend::getUniformLocation(ShaderHandle shader,
                                        const std::string& name) {
    // Cache key: program id in the high 32 bits, name hash in the low.
    // glGetUniformLocation is a driver round-trip; calling it three times
    // per frame per shader is measurable once a scene grows.
    const uint64_t key = (static_cast<uint64_t>(shader.id) << 32)
                       | (std::hash<std::string>{}(name) & 0xFFFFFFFF);

    auto it = m_uniformCache.find(key);
    if (it != m_uniformCache.end()) {
        return it->second;
    }

    const GLint location = glGetUniformLocation(shader.id, name.c_str());
    m_uniformCache[key] = location;
    return location;
}

void OpenGLBackend::setUniformMat4(ShaderHandle shader,
                                   const std::string& name,
                                   const glm::mat4& value) {
    glUniformMatrix4fv(getUniformLocation(shader, name), 1, GL_FALSE,
                       glm::value_ptr(value));
}

void OpenGLBackend::setUniformVec3(ShaderHandle shader,
                                   const std::string& name,
                                   const glm::vec3& value) {
    glUniform3fv(getUniformLocation(shader, name), 1, glm::value_ptr(value));
}

void OpenGLBackend::setUniformFloat(ShaderHandle shader,
                                    const std::string& name,
                                    float value) {
    glUniform1f(getUniformLocation(shader, name), value);
}

// --- Draw --------------------------------------------------------------

void OpenGLBackend::draw(VertexArrayHandle vao, uint32_t vertexCount) {
    glBindVertexArray(vao.id);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount));
    glBindVertexArray(0);
}

void OpenGLBackend::drawIndexed(VertexArrayHandle vao, uint32_t indexCount) {
    glBindVertexArray(vao.id);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount),
                   GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

// --- Frame / state -----------------------------------------------------

void OpenGLBackend::setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void OpenGLBackend::clear(const glm::vec4& color, bool clearDepth) {
    glClearColor(color.r, color.g, color.b, color.a);
    GLbitfield mask = GL_COLOR_BUFFER_BIT;
    if (clearDepth) {
        mask |= GL_DEPTH_BUFFER_BIT;
    }
    glClear(mask);
}

void OpenGLBackend::setDepthTest(bool enabled) {
    if (enabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

} // namespace rhi
