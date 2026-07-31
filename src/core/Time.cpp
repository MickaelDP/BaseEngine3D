#include "Time.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Maximum delta time accepted, in seconds.
// A frame can stall arbitrarily long (window being dragged, debugger
// breakpoint, OS suspend). Without a clamp, the next frame gets a huge dt
// and everything driven by it teleports — the classic "tunneling" bug.
// Better to run one deliberately slow frame than to jump through walls.
static constexpr float MAX_DELTA_TIME = 0.1f;

Time::Time() {
    // glfwGetTime() returns seconds since glfwInit(), as a double.
    // A Window must already exist (GLFW initialized) before constructing
    // a Time object, otherwise this reads 0 and the first dt is wrong.
    m_lastTime = glfwGetTime();
    m_currentTime = m_lastTime;
}

void Time::update() {
    m_currentTime = glfwGetTime();

    // double subtraction first, then narrow to float: doing the maths in
    // double avoids precision loss on a timer that keeps growing, while
    // float is plenty for the small per-frame value we hand out.
    double raw = m_currentTime - m_lastTime;
    m_lastTime = m_currentTime;

    m_deltaTime = static_cast<float>(raw);
    if (m_deltaTime > MAX_DELTA_TIME) {
        m_deltaTime = MAX_DELTA_TIME;
    }

    // Accumulate the RAW value, not the clamped one: the FPS report should
    // describe what actually happened, including the stall.
    m_fpsAccumulator += raw;
    m_frameCount++;

    m_secondElapsed = false;
    if (m_fpsAccumulator >= 1.0) {
        m_fps = static_cast<float>(m_frameCount / m_fpsAccumulator);
        m_frameCount = 0;
        m_fpsAccumulator = 0.0;
        m_secondElapsed = true;
    }
}