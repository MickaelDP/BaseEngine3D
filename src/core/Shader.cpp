#include "core/Shader.h"

Shader::Shader(rhi::IRenderBackend& backend,
               const char* vertexSrc,
               const char* fragmentSrc)
    : m_backend(backend) {
    m_handle = m_backend.createShader(vertexSrc, fragmentSrc);
}

Shader::~Shader() {
    m_backend.destroyShader(m_handle);
}

void Shader::use() const {
    m_backend.bindShader(m_handle);
}

void Shader::setMat4(const std::string& name, const glm::mat4& m) const {
    m_backend.setUniformMat4(m_handle, name, m);
}

void Shader::setVec3(const std::string& name, const glm::vec3& v) const {
    m_backend.setUniformVec3(m_handle, name, v);
}

void Shader::setFloat(const std::string& name, float value) const {
    m_backend.setUniformFloat(m_handle, name, value);
}
