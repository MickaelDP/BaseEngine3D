#pragma once

#include "core/Window.h"
#include "core/Time.h"
#include "core/ISubsystem.h"

#include <memory>
#include <string>
#include <vector>

// Step 6: the orchestrator.
// Single responsibility: own the application lifecycle and drive the
// subsystems in a guaranteed order. Contains no rendering, no game logic.
//
// Window and Time are NOT subsystems: they are infrastructure the loop
// itself depends on. Time must tick before any subsystem updates, and
// Window must swap buffers after all of them have rendered — neither fits
// the "one of many, order-agnostic" model of ISubsystem.
class Engine {
    public:
        Engine(int width, int height, const std::string& title);
        ~Engine();

        // Non-copyable: owns the window and every subsystem.
        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        // Register a subsystem. Must be called BEFORE run().
        // Engine takes ownership; init() order follows registration order.
        void addSubsystem(std::unique_ptr<ISubsystem> subsystem);

        // Init all subsystems, run the main loop, shut everything down.
        void run();

        // Subsystems need these to do their work.
        Window& getWindow() { return m_window; }
        const Time& getTime() const { return m_time; }

    private:
        void initSubsystems();
        void shutdownSubsystems();

        Window m_window;
        Time   m_time;

        std::vector<std::unique_ptr<ISubsystem>> m_subsystems;

        // Guards against addSubsystem() being called mid-run, which would
        // register a subsystem whose init() never gets called.
        bool m_running = false;
};