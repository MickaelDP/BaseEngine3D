#pragma once

#include <cstdint>

// Step 7: opaque resource handles.
//
// The whole point of this file: nothing above the backend layer may ever
// see a GLuint. A handle is an opaque integer whose meaning is known only
// to the backend that issued it. Swapping OpenGLBackend for VulkanBackend
// changes what the number refers to internally, and nothing else.
//
// Distinct struct types rather than plain typedefs: with `using
// BufferHandle = uint32_t`, passing a shader handle where a buffer is
// expected compiles silently. With distinct types, it's a compile error.
// This is the "strong typedef" idiom.

namespace rhi {

// Value reserved for "no resource". Backends never issue 0 as a valid id.
inline constexpr uint32_t INVALID_ID = 0;

struct BufferHandle {
    uint32_t id = INVALID_ID;
    bool isValid() const { return id != INVALID_ID; }
};

struct VertexArrayHandle {
    uint32_t id = INVALID_ID;
    bool isValid() const { return id != INVALID_ID; }
};

struct ShaderHandle {
    uint32_t id = INVALID_ID;
    bool isValid() const { return id != INVALID_ID; }
};

// What the buffer is for. The backend maps this to its own constants
// (GL_ARRAY_BUFFER here, VkBufferUsageFlags in a Vulkan backend).
enum class BufferType {
    Vertex,
    Index,
    Storage      // SSBO — needed for the compute/baking pipeline later
};

// Expected access pattern, a hint for memory placement.
enum class BufferUsage {
    Static,      // written once, read many times
    Dynamic      // rewritten frequently
};

// Element type of a vertex attribute.
enum class AttributeFormat {
    Float2,
    Float3,
    Float4
};

// One vertex attribute description, backend-neutral.
struct VertexAttribute {
    uint32_t        location;   // matches layout(location = N) in the shader
    AttributeFormat format;
    uint32_t        offset;     // byte offset within the vertex struct
};

// How to interpret the bytes of a vertex buffer.
struct VertexLayout {
    const VertexAttribute* attributes = nullptr;
    uint32_t               count      = 0;
    uint32_t               stride     = 0;  // bytes between two vertices
};

} // namespace rhi