#ifndef TRIANGLEDEMO_H
#define TRIANGLEDEMO_H

#include "ImDemoInterface.h"

namespace IME
{
    class ShaderProgram;
    class TrisObject;

    class TriangleDemo : public ImDemoInterface
    {
    public:
        ~TriangleDemo() override;

        void OnInit() override;

        void OnRender(float _aspect) override;

        void OnImGuiRender() override;

    private:
        void SetupTriangle();

        ShaderProgram* m_shader = nullptr;
        TrisObject* m_tri = nullptr;
        float m_rotationSpeed = 0.5f;
        float m_rotationAngleRad = 0.0f; // Cumulative rotation progress in radians
        float m_lastFrameTime = 0.0f; // Time at the last frame

    };

    class TriangleDemoFactory : public ImDemoFactory
    {
    public:
        ~TriangleDemoFactory() override;
        std::string GetName() const override;
        ImDemoInterface* CreateDemo() override;
    };
}

#endif //TRIANGLEDEMO_H