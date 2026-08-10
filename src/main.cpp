#include "core/Engine.h"
#include "render/RenderSubsystem.h"

#include <iostream>
#include <memory>

// Step 7: unchanged in spirit from Step 6 — the subsystem now receives
// the backend from the Engine. main() still knows nothing about OpenGL.
int main() {
    try
    {
        constexpr int WIDTH  = 800;
        constexpr int HEIGHT = 600;

        Engine engine(WIDTH, HEIGHT, "BaseEngine3D - Step 7");

        const float aspect = static_cast<float>(WIDTH)
                           / static_cast<float>(HEIGHT);

        engine.addSubsystem(
            std::make_unique<RenderSubsystem>(engine.getBackend(), aspect));

        engine.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
