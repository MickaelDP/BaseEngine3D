#pragma once

#include "core/ISubsystem.h"
#include "core/Shader.h"
#include "core/Mesh.h"
#include "scene/Camera.h"
#include "scene/Transform.h"
#include "rhi/IRenderBackend.h"

#include <memory>

// Step 7: the subsystem now receives a backend reference instead of
// calling GL itself. Note there is no #include <glad/glad.h> anywhere in
// this file or its .cpp — that is the measurable outcome of this step.
class RenderSubsystem : public ISubsystem {
    public:
        RenderSubsystem(rhi::IRenderBackend& backend, float aspectRatio);
        ~RenderSubsystem() override = default;

        void init() override;
        void update(float dt) override;
        void shutdown() override;

        const char* getName() const override { return "RenderSubsystem"; }

    private:
        rhi::IRenderBackend& m_backend;
        float m_aspectRatio;

        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<Mesh>   m_cube;
        std::unique_ptr<Camera> m_camera;

        Transform m_cubeTransform;
};
