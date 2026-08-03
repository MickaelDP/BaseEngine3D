#include "core/Engine.h"

#include <glad/glad.h>
#include <iostream>
#include <stdexcept>

Engine::Engine(int width, int height, const std::string& title)
    : m_window(width, height, title), m_time() {
    // Member init order matters and is determined by DECLARATION order in
    // the header, not by the order written here: m_window is declared
    // first, so it is constructed first. Time's constructor calls
    // glfwGetTime(), which requires GLFW to be initialized — which
    // Window's constructor did.
    std::cout << "[Engine] initialized (" << width << "x" << height << ")"
              << std::endl;
}

Engine::~Engine() {
    // Subsystems are destroyed here via unique_ptr, in reverse vector
    // order. shutdown() has already been called explicitly in run(), so
    // this is just memory cleanup.
    std::cout << "[Engine] destroyed" << std::endl;
}

void Engine::addSubsystem(std::unique_ptr<ISubsystem> subsystem) {
    if (m_running) {
        throw std::runtime_error(
            "Cannot add a subsystem while the engine is running"
        );
    }
    std::cout << "[Engine] registered subsystem: "
              << subsystem->getName() << std::endl;
    m_subsystems.push_back(std::move(subsystem));
}

void Engine::initSubsystems() {
    // Forward order: a subsystem may depend on one registered before it.
    for (auto& sub : m_subsystems) {
        std::cout << "[Engine] init: " << sub->getName() << std::endl;
        sub->init();
    }
}

void Engine::shutdownSubsystems() {
    // REVERSE order, mirroring init. If subsystem B was built on top of
    // A's resources, B must release them before A tears them down.
    // Same principle as destructors unwinding a stack.
    for (auto it = m_subsystems.rbegin(); it != m_subsystems.rend(); ++it) {
        std::cout << "[Engine] shutdown: " << (*it)->getName() << std::endl;
        (*it)->shutdown();
    }
}

void Engine::run() {
    m_running = true;
    initSubsystems();

    while (!m_window.shouldClose()) {
        // --- 1. Timing ---
        m_time.update();
        const float dt = m_time.getDeltaTime();

        // --- 2. Input ---
        m_window.pollEvents();

        // --- 3. Clear ---
        // The Engine owns the frame boundaries. Subsystems draw into an
        // already-cleared buffer and never call glClear themselves —
        // otherwise a second subsystem would erase the first one's work.
        glClearColor(0.1f, 0.12f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- 4. Update every subsystem ---
        for (auto& sub : m_subsystems) {
            sub->update(dt);
        }

        // --- 5. Present ---
        m_window.swapBuffers();

        if (m_time.hasSecondElapsed()) {
            std::cout << "FPS: " << m_time.getFPS() << std::endl;
        }
    }

    shutdownSubsystems();
    m_running = false;
}