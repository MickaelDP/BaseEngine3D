#include "Shader.h"

#include <stdexcept>
#include <string>
#include <vector>

// Helper: compile one shader stage (vertex or fragment).
// Returns the GL handle on success, throws with the GLSL log on failure.
GLuint Shader::compileStage(GLenum type, const char* src) {
    // 1. Create an empty shader object on the GPU for the given stage type
    GLuint shader = glCreateShader(type);

    // 2. Attach the raw GLSL source code string to the shader object
    glShaderSource(shader, 1, &src, nullptr);

    // 3. Compile the GLSL source code into GPU-executable code
    glCompileShader(shader);

    // Query compilation status. Never assume success: driver bugs, GLSL
    // version mismatches, or a simple typo all produce silent failures
    // without this check.
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetShaderInfoLog(shader, logLen, nullptr, log.data());

        // Clean up the broken shader object to prevent GPU memory leaks
        glDeleteShader(shader);
        throw std::runtime_error(
            std::string("Shader compilation failed:\n") + log.data()
        );
    }

    return shader;
}

Shader::Shader(const char* vertexSrc, const char* fragmentSrc) {
    // Compile both stages first. If either throws, nothing has been linked
    // yet so there's no program handle to clean up.
    GLuint vert = compileStage(GL_VERTEX_SHADER,   vertexSrc);
    GLuint frag = compileStage(GL_FRAGMENT_SHADER, fragmentSrc);
  
    // 1. Create an empty container program on the GPU to hold our shaders
    m_id = glCreateProgram();
    // 2. Attach the compiled vertex shader to this program
    glAttachShader(m_id, vert);
    // 3. Attach the compiled fragment shader to this program
    glAttachShader(m_id, frag);
    // 4. Link the program together, merging the vertex and fragment stages for the GPU pipeline
    glLinkProgram(m_id);

    // Shader objects are no longer needed once linked into the program.
    // Deleting them here frees GPU memory without affecting the program.
    glDeleteShader(vert);
    glDeleteShader(frag);

    // Link errors are a separate check from compile errors — a program can
    // compile cleanly but fail to link (e.g. missing main(), mismatched
    // in/out between stages).
    GLint success = 0;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLen = 0;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetProgramInfoLog(m_id, logLen, nullptr, log.data());
        glDeleteProgram(m_id);
        throw std::runtime_error(
            std::string("Shader linking failed:\n") + log.data()
        );
    }
}

Shader::~Shader() {
    // glDeleteProgram(0) is a no-op per the GL spec, so this is safe even
    // if construction failed and m_id was never assigned.
    glDeleteProgram(m_id);
}

void Shader::use() const {
    glUseProgram(m_id);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& v) const {
    glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(v));
}

void Shader::setMat4(const std::string& name, const glm::mat4& m) const {
    glUniformMatrix4fv(
        glGetUniformLocation(m_id, name.c_str()),
        1, GL_FALSE, glm::value_ptr(m)
    );
}