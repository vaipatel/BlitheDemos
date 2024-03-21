#ifndef TRIANGLEDEMO_H
#define TRIANGLEDEMO_H

#include "DemoInterface.h"

namespace BLE
{
    class ShaderProgram;
    class Texture;
    class TrisObject;

    class TriangleDemo : public DemoInterface
    {
    public:
        ~TriangleDemo() override;

        void OnInit() override;

        void OnRender(const UIData& _uiData, const UIData* _defaultViewPortUIData = nullptr) override;

        void OnDrawUI() override;

        bool UsesDefaultScene() const override { return true; }

    private:
        void SetupTriangle();

        ShaderProgram* m_shader = nullptr;
        Texture* m_texture = nullptr;
        TrisObject* m_tri = nullptr;
        float m_rotationSpeed = 0.5f;
        float m_rotationAngleRad = 0.0f; // Cumulative rotation progress in radians
        float m_lastFrameTime = 0.0f; // Time at the last frame

    };

    class TriangleDemoFactory : public DemoFactory
    {
    public:
        ~TriangleDemoFactory() override;
        std::string GetName() const override;
        DemoInterface* CreateDemo() override;
    };
}

#endif //TRIANGLEDEMO_H