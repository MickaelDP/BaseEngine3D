#pragma once

// Step 2: frame timing.
// Single responsibility: measure how much real time elapsed between two
// frames (delta time), and expose a once-per-second FPS report.
//
// Note: no GLFW/glad include here on purpose. The header only exposes
// plain numbers, so nothing that includes Time.h drags the whole OpenGL
// header chain along. The GLFW dependency stays hidden in Time.cpp.
class Time {
    public:
        Time();

        // Call exactly once per frame, at the very top of the loop.
        void update();

        // Seconds elapsed since the previous frame. Everything that moves
        // (camera, animation, physics) must be multiplied by this, so the
        // engine behaves identically at 30 or 300 FPS.
        float getDeltaTime() const { return m_deltaTime; }

        // Seconds elapsed since the Time object was created.
        double getElapsedTime() const { return m_currentTime; }

        // True on the single frame where a new full second was reached.
        // Used to print stats without flooding the console every frame.
        bool hasSecondElapsed() const { return m_secondElapsed; }

        // Average frames per second over the last completed second.
        float getFPS() const { return m_fps; }

    private:
        double m_lastTime = 0.0;
        double m_currentTime = 0.0;
        float  m_deltaTime = 0.0f;

        // FPS accumulation over a one-second window
        double m_fpsAccumulator = 0.0;
        int    m_frameCount = 0;
        float  m_fps = 0.0f;
        bool   m_secondElapsed = false;
};