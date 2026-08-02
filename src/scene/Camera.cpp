#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

// World up vector. Fixed to +Y: this engine treats Y as "up", which is
// the OpenGL/GLM convention (Unreal and 3ds Max use Z-up instead — a
// classic source of confusion when importing assets).
static const glm::vec3 WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);

Camera::Camera(const glm::vec3& position, float aspectRatio)
    : m_position(position), m_aspectRatio(aspectRatio) {
}

void Camera::setRotation(float yawDeg, float pitchDeg) {
    m_yaw = yawDeg;

    // Clamp pitch just shy of ±90°. At exactly 90° the forward vector
    // becomes parallel to WORLD_UP, their cross product collapses to
    // zero, and the view matrix degenerates — the camera flips wildly.
    // This is the practical answer to gimbal lock for an FPS camera.
    m_pitch = std::clamp(pitchDeg, -89.0f, 89.0f);
}

glm::vec3 Camera::getForward() const {
    // Spherical to cartesian conversion.
    const float yawRad   = glm::radians(m_yaw);
    const float pitchRad = glm::radians(m_pitch);

    glm::vec3 forward;
    forward.x = std::cos(yawRad) * std::cos(pitchRad);
    forward.y = std::sin(pitchRad);
    forward.z = std::sin(yawRad) * std::cos(pitchRad);

    return glm::normalize(forward);
}

glm::mat4 Camera::getViewMatrix() const {
    // glm::lookAt builds the inverse of the camera's world transform.
    // Counter-intuitive but correct: moving the camera right is
    // mathematically identical to moving the entire world left.
    return glm::lookAt(m_position, m_position + getForward(), WORLD_UP);
}

glm::mat4 Camera::getProjectionMatrix() const {
    // Perspective projection: objects shrink with distance.
    // The alternative, glm::ortho, keeps them the same size — that's what
    // a 2D engine (or a CAD view) uses.
    return glm::perspective(
        glm::radians(m_fovDegrees),
        m_aspectRatio,
        m_near,
        m_far
    );
}