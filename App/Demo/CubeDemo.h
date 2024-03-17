#ifndef CUBEDEMO_H
#define CUBEDEMO_H

#include "ImDemoInterface.h"

namespace IME
{
    class ShaderProgram;
    class MeshObject;

    class CubeDemo : public ImDemoInterface
    {
    public:
        ~CubeDemo() override;

        void OnInit() override;

        void OnRender(const UIData& _uiData) override;

        void OnImGuiRender() override;

    private:
        void SetupCube();

        ShaderProgram* m_shader = nullptr;
        MeshObject* m_cube = nullptr;
        float m_rotationSpeed = 0.5f;
        float m_rotationAngleRad = 0.0f; // Cumulative rotation progress in radians
        float m_lastFrameTime = 0.0f; // Time at the last frame

    };

    class CubeDemoFactory : public ImDemoFactory
    {
    public:
        ~CubeDemoFactory() override;
        std::string GetName() const override;
        ImDemoInterface* CreateDemo() override;
    };
}

#endif //CUBEDEMO_H