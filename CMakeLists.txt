cmake_minimum_required(VERSION 3.20)
project(BaseEngine3D LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# --- GLFW (windowing + context) ---
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Disable Wayland and force X11: Wayland is unnecessary for this project 
# and can cause compilation/dependency complications depending on the environment context.
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

# --- GLM (math, header-only) ---
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# --- glad (vendored: manually generated, no FetchContent) ---
add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# --- Main executable ---
add_executable(BaseEngine3D
    src/main.cpp
    src/core/Window.cpp
    src/core/Time.cpp
)
target_link_libraries(BaseEngine3D PRIVATE glad glfw glm::glm)

# glad uses dlopen/dlsym under Linux to load GL functions at
# runtime: libdl must be explicitly linked, otherwise link failure.
if(UNIX AND NOT APPLE)
    target_link_libraries(BaseEngine3D PRIVATE ${CMAKE_DL_LIBS})
endif()