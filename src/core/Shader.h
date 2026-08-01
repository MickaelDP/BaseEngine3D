#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

// Step 3: GLSL shader program wrapper.
// Single responsibility: compile a vertex + fragment shader from source
// strings, link them into a GL program, and expose use() + setUniform().
//
// The class owns the GL program handle (m_id). It is non-copyable for the
// same reason as Window: copying a raw GL handle would double-delete on
// destruction.
class Shader {
    public:
        // Compile and link from raw GLSL source strings (not file paths).
        // File loading is a separate concern and belongs in a future
        // ResourceManager, not here.
        Shader(const char* vertexSrc, const char* fragmentSrc);
        ~Shader();

        // Non-copyable: owns a GL program handle.
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;


        // Bind this program for all subsequent draw calls.
        void use() const;

        // Uniform setters — add more types as needed in future steps.
        // Marked 'const' because these methods modify the global OpenGL state
        // but do not modify the internal state of the C++ Shader object.
        
        // int / float : Simple numerical values (flags, counters, intensities)
        void setInt  (const std::string& name, int value)            const;
        void setFloat(const std::string& name, float value)          const;
        // vec3 : 3-component vector (3D positions x, y, z or RGB colors)
        void setVec3 (const std::string& name, const glm::vec3& v)   const;
        // mat4 : 4x4 Matrix (Absolute foundation of 3D: Model, View, Projection) 
        void setMat4 (const std::string& name, const glm::mat4& m)   const;

        // Returns the raw OpenGL identifier (GLuint) for external systems or debugging
        GLuint getId() const { return m_id; }

    private:
        GLuint m_id = 0;

        // Compile a single shader stage and return its handle.
        // Throws std::runtime_error with the GLSL log on failure.
        static GLuint compileStage(GLenum type, const char* src);
};