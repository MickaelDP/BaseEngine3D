#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Step 0: nothing functional yet.
// Unique goal: prove that the 3 dependencies include and link correctly.
// No window, no logic.
//
// Include order is critical: glad BEFORE GLFW.
// glad.h defines the true OpenGL prototypes; if GLFW/glfw3.h is included
// first, it includes its own minimal GL header -> symbol redefinitions
// at compile time.
int main() {
    return 0;
}