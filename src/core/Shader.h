#pragma once

#include "rhi/IRenderBackend.h"

#include <glm/glm.hpp>
#include <string>

// Step 7: Shader is now a thin, backend-agnostic wrapper.
//
// All the GLSL compilation logic moved into OpenGLBackend::createShader.
// What remains here is a lifetime guard around a ShaderHandle plus a
// convenience API. That's deliberate: compilation is API-specific
// (Vulkan consumes pre-compiled SPIR-V), so it belongs to the backend.
class Shader {
    public:
        Shader(rhi::IRenderBackend& backend,
               const char* vertexSrc,
               const char* fragmentSrc);
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        void use() const;

        void setMat4 (const std::string& name, const glm::mat4& m) const;
        void setVec3 (const std::string& name, const glm::vec3& v) const;
        void setFloat(const std::string& name, float value)        const;

    private:
        rhi::IRenderBackend& m_backend;
        rhi::ShaderHandle    m_handle;
};
