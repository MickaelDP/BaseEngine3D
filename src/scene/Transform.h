#pragma once

#include <glm/glm.hpp>

// Step 5: object placement in world space.
// Single responsibility: hold position/rotation/scale and produce the
// Model matrix on demand. Knows nothing about rendering or GL.
//
// Rotation is stored as Euler angles in DEGREES (pitch/yaw/roll on
// x/y/z). Degrees rather than radians because that's what a human types
// when authoring a scene; the conversion happens inside getModelMatrix().
// Quaternions would avoid gimbal lock but are overkill here — see the
// note in the accompanying doc.
class Transform {
    public:
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);  // degrees, XYZ
        glm::vec3 scale    = glm::vec3(1.0f);

        // Build the Model matrix: scale, then rotate, then translate.
        // The multiplication order below is the REVERSE of that sentence
        // because matrix multiplication applies right-to-left.
        glm::mat4 getModelMatrix() const;
};