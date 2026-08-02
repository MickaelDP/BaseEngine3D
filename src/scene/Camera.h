#pragma once

#include <glm/glm.hpp>

// Step 5: the 3D camera. THE structural difference with a 2D engine.
//
// A 2D engine needs one orthographic transform. A 3D engine needs two
// separate matrices:
//   - View       : where the camera is and what it looks at
//   - Projection : how 3D space is flattened onto the 2D screen
//
// Orientation uses yaw/pitch in degrees rather than quaternions. Gimbal
// lock is avoided in practice by clamping pitch to ±89°, which is what
// every FPS camera does. Quaternions become necessary for free-flight
// (space sim) cameras, not for this.
class Camera {
    public:
        Camera(const glm::vec3& position, float aspectRatio);

        // Where the camera is, and where it looks.
        glm::mat4 getViewMatrix() const;

        // How the frustum flattens 3D onto the screen.
        glm::mat4 getProjectionMatrix() const;

        void setPosition(const glm::vec3& p) { m_position = p; }
        glm::vec3 getPosition() const { return m_position; }

        // Called on window resize: the projection depends on the aspect
        // ratio, so a stale value stretches the whole scene.
        void setAspectRatio(float ratio) { m_aspectRatio = ratio; }

        // yaw/pitch in degrees. Pitch is clamped internally.
        void setRotation(float yawDeg, float pitchDeg);

        // Unit vector the camera currently faces.
        glm::vec3 getForward() const;

    private:
        glm::vec3 m_position;

        float m_yaw   = -90.0f;  // -90° so the default forward is -Z
        float m_pitch =   0.0f;

        float m_aspectRatio;
        float m_fovDegrees = 45.0f;

        // Frustum bounds. Nothing closer than near or further than far is
        // drawn. A very small near value wrecks depth buffer precision —
        // see "z-fighting" in the accompanying doc.
        float m_near = 0.1f;
        float m_far  = 100.0f;
};