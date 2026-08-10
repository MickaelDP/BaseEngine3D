#pragma once

#include "core/Window.h"
#include "core/Time.h"
#include "core/ISubsystem.h"
#include "rhi/IRenderBackend.h"

#include <memory>
#include <string>
#include <vector>

// Step 7: the Engine now owns the render backend.
//
// It is created as a concrete OpenGLBackend inside the constructor, but
// stored as a unique_ptr<IRenderBackend>. Switching to Vulkan later means
// changing one line in Engine.cpp — every subsystem receives the
// interface, not the implementation.
class Engine {
    public:
        Engine(int width, int height, const std::string& title);
        ~Engine();

        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        void addSubsystem(std::unique_ptr<ISubsystem> subsystem);
        void run();

        Window& getWindow() { return m_window; }
        const Time& getTime() const { return m_time; }

        // Subsystems get the interface, never the concrete backend.
        rhi::IRenderBackend& getBackend() { return *m_backend; }

    private:
        void initSubsystems();
        void shutdownSubsystems();

        // Declaration order IS construction order: Window first (it
        // creates the GL context), then the backend that depends on it.
        Window m_window;
        Time   m_time;

        std::unique_ptr<rhi::IRenderBackend> m_backend;
        std::vector<std::unique_ptr<ISubsystem>> m_subsystems;

        bool m_running = false;
};
