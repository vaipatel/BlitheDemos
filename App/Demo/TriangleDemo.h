#ifndef TRIANGLEDEMO_H
#define TRIANGLEDEMO_H

#include "ImDemoInterface.h"

namespace IME
{
    class ShaderProgram;
	class Texture;
    class TrisObject;

    class TriangleDemo : public ImDemoInterface
    {
    public:
        ~TriangleDemo() override;

        void OnInit() override;

        void OnRender(const UIData& _uiData, const UIData* _defaultViewPortUIData = nullptr) override;

        void OnImGuiRender() override;

		bool UsesDefaultScene() const override { return true;  }

    private:
        void SetupTriangle();

        ShaderProgram* m_shader = nullptr;
		Texture* m_texture = nullptr;
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