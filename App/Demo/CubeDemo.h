#ifndef CUBEDEMO_H
#define CUBEDEMO_H

#include "DemoInterface.h"

namespace blithe
{
    class CachedMeshObject;
    class IGLBufferCache;
    class ShaderProgram;
    struct Mesh;
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
        void SetupCubeMesh();
        void ProcessKeys(const UIData& _uiData, float _deltaTime);
        void ProcessMouseMove(const UIData& _uiData, float _deltaTime);

        ShaderProgram* m_shader = nullptr;
        Mesh* m_cubeMesh = nullptr;
        CachedMeshObject* m_cachedCube = nullptr;
        IGLBufferCache* m_glBufferCache = nullptr;
        Texture* m_texture = nullptr;
        CameraDecorator* m_cameraDecorator = nullptr;
        float m_rotationSpeed = 0.5f;   //!< Value from UI control for the Rotation Speed
        bool m_useCustomAspect = false; //!< Value from UI control for whether the custom aspect ratio is used
        float m_customAspect = 1.0f;    //!< Value from UI control for the custom aspect ratio

        float m_rotationAngleRad = 0.0f; // Cumulative rotation progress in radians
    };

    DECLARE_DEMO(CubeDemo, "Cube Demo");
}

#endif //CUBEDEMO_H
