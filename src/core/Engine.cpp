#include "core/Engine.h"
#include "rhi/OpenGLBackend.h"

#include <iostream>
#include <stdexcept>

Engine::Engine(int width, int height, const std::string& title)
    : m_window(width, height, title), m_time() {

    // THE line to change for a Vulkan port. Everything downstream sees
    // only IRenderBackend, so nothing else in the engine is affected.
    m_backend = std::make_unique<rhi::OpenGLBackend>();
    m_backend->init();
    m_backend->setViewport(0, 0, width, height);

    std::cout << "[Engine] initialized (" << width << "x" << height << ")"
              << std::endl;
}

Engine::~Engine() {
    // Backend shutdown must happen while the GL context is still alive,
    // i.e. before Window's destructor runs. Members are destroyed in
    // reverse declaration order, so m_backend goes before m_window —
    // but the explicit shutdown() in run() is what actually guarantees
    // correct ordering of resource release.
    std::cout << "[Engine] destroyed" << std::endl;
}

void Engine::addSubsystem(std::unique_ptr<ISubsystem> subsystem) {
    if (m_running) {
        throw std::runtime_error(
            "Cannot add a subsystem while the engine is running");
    }
    std::cout << "[Engine] registered subsystem: "
              << subsystem->getName() << std::endl;
    m_subsystems.push_back(std::move(subsystem));
}

void Engine::initSubsystems() {
    for (auto& sub : m_subsystems) {
        std::cout << "[Engine] init: " << sub->getName() << std::endl;
        sub->init();
    }
}

void Engine::shutdownSubsystems() {
    for (auto it = m_subsystems.rbegin(); it != m_subsystems.rend(); ++it) {
        std::cout << "[Engine] shutdown: " << (*it)->getName() << std::endl;
        (*it)->shutdown();
    }
}

void Engine::run() {
    m_running = true;
    initSubsystems();

    while (!m_window.shouldClose()) {
        m_time.update();
        const float dt = m_time.getDeltaTime();

        m_window.pollEvents();

        // No gl* call here any more — the Engine asks the backend.
        m_backend->clear(glm::vec4(0.1f, 0.12f, 0.18f, 1.0f), true);

        for (auto& sub : m_subsystems) {
            sub->update(dt);
        }

        m_window.swapBuffers();

        if (m_time.hasSecondElapsed()) {
            std::cout << "FPS: " << m_time.getFPS() << std::endl;
        }
    }

    shutdownSubsystems();

    // Backend teardown before the Window (and thus the GL context) dies.
    m_backend->shutdown();
    m_running = false;
}
