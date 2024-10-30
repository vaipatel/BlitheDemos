#ifndef CUBEDEMO_H
#define CUBEDEMO_H

#include "DemoInterface.h"

namespace blithe
{
    class ShaderProgram;
    class MeshObject;
    class Texture;
    class CameraDecorator;

    class CubeDemo : public DemoInterface
    {
    public:
        ~CubeDemo() override;

        void OnInit() override;

        void OnRender(double _deltaTimeS, const UIData& _uiData) override;

        void OnDrawUI() override;

        bool UsesStandardViewPort() const override { return true; }

    private:
        void SetupCube();
        void ProcessKeys(const UIData& _uiData, float _deltaTime);
        void ProcessMouseMove(const UIData& _uiData, float _deltaTime);

        ShaderProgram* m_shader = nullptr;
        MeshObject* m_cube = nullptr;
        Texture* m_texture = nullptr;
        CameraDecorator* m_cameraDecorator = nullptr;
        float m_rotationSpeed = 0.5f;   //!< Value from UI control for the Rotation Speed
        bool m_useCustomAspect = false; //!< Value from UI control for whether the custom aspect ratio is used
        float m_customAspect = 1.0f;    //!< Value from UI control for the custom aspect ratio

        float m_rotationAngleRad = 0.0f; // Cumulative rotation progress in radians
    };

    class CubeDemoFactory : public DemoFactory
    {
    public:
        ~CubeDemoFactory() override;
        std::string GetName() const override;
        DemoInterface* CreateDemo() override;
    };
}

#endif //CUBEDEMO_H
