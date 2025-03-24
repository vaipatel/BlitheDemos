#ifndef TRIANGLEDEMO_H
#define TRIANGLEDEMO_H

#include "DemoInterface.h"

namespace blithe
{
    class ShaderProgram;
    class Texture;
    class TrisObject;

    class TriangleDemo : public DemoInterface
    {
    public:
        ~TriangleDemo() override;

        void OnInit() override;

        void OnRender(double _deltaTimeS, const UIData& _uiData) override;

        void OnDrawUI() override;

        bool UsesStandardViewPort() const override { return true; }

    private:
        void SetupTriangle();

        ShaderProgram* m_shader = nullptr;
        Texture* m_texture = nullptr;
        TrisObject* m_tri = nullptr;
        float m_rotationSpeed = 0.5f;
        float m_rotationAngleRad = 0.0f; // Cumulative rotation progress in radians
    };

    DECLARE_DEMO(TriangleDemo, "Triangle Demo");
}

#endif //TRIANGLEDEMO_H
