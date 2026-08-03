#pragma once

#include "core/ISubsystem.h"
#include "core/Shader.h"
#include "core/Mesh.h"
#include "scene/Camera.h"
#include "scene/Transform.h"

#include <memory>

// Step 6: the first concrete subsystem.
// Owns everything the Step 5 main() used to own directly: shader, mesh,
// camera, transform. The Engine drives it without knowing any of that.
//
// Members are held by unique_ptr rather than by value because Shader,
// Mesh and Camera all need an active GL context to construct — which
// only exists once the Engine has built its Window. Constructing them in
// init() rather than in the constructor is what makes that possible.
class RenderSubsystem : public ISubsystem {
    public:
        explicit RenderSubsystem(float aspectRatio);
        ~RenderSubsystem() override = default;

        void init() override;
        void update(float dt) override;
        void shutdown() override;

        const char* getName() const override { return "RenderSubsystem"; }

    private:
        float m_aspectRatio;

        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<Mesh>   m_cube;
        std::unique_ptr<Camera> m_camera;

        Transform m_cubeTransform;
};