#include "core/Engine.h"
#include "render/RenderSubsystem.h"

#include <iostream>
#include <memory>

// Step 6: main() is now what it should always have been — an entry point
// and nothing else. It instantiates the Engine, registers subsystems,
// calls run(), and catches fatal errors.
//
// Compare with Step 5's main(): shaders, cube data, camera, matrices and
// the render loop all lived here. If application logic starts creeping
// back into this file, that's the signal a responsibility has leaked out
// of the Engine layer.
int main() {
    try
    {
        constexpr int WIDTH  = 800;
        constexpr int HEIGHT = 600;

        Engine engine(WIDTH, HEIGHT, "BaseEngine3D - Step 6");

        const float aspect = static_cast<float>(WIDTH)
                           / static_cast<float>(HEIGHT);

        engine.addSubsystem(std::make_unique<RenderSubsystem>(aspect));

        engine.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}