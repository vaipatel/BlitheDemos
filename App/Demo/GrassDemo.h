#ifndef GRASSDEMO_H
#define GRASSDEMO_H

#include <vector>
#include <glm/glm.hpp>
#include "DemoInterface.h"

namespace blithe
{
    class CameraDecorator;
    class Mesh;
    class MeshObject;
    class ShaderProgram;
    class Texture;

    class GrassDemo : public DemoInterface
    {
    public:
        ~GrassDemo() override;

        void OnInit() override;

        void OnRender(double _deltaTimeS, const UIData& _uiData) override;

        void OnDrawUI() override;

        bool UsesStandardViewPort() const override { return true; }

    private:
        void SetupCube();
        void ProcessKeys(const UIData& _uiData, float _deltaTime);
        void ProcessMouseMove(const UIData& _uiData, float _deltaTime);
        std::vector<glm::mat4> GenerateRandomModelMatrices(int _count, float _maxTranslation, float _minScale, float _maxScale);
        std::vector<glm::mat4> GenerateGridModelMatrices(int _xCount, int _yCount, int _zCount, float _spacing);

        void SetupGrass(const std::string& _grassFileName);
        bool Load3DFile(const std::string& _filename, Mesh& _outMesh);

        ShaderProgram* m_shader = nullptr;
        MeshObject* m_cube = nullptr;
        MeshObject* m_grass = nullptr;
        Texture* m_texture = nullptr;
        CameraDecorator* m_cameraDecorator = nullptr;
        float m_rotationSpeed = 0.5f;   //!< Value from UI control for the Rotation Speed
        bool m_useCustomAspect = false; //!< Value from UI control for whether the custom aspect ratio is used
        float m_customAspect = 1.0f;    //!< Value from UI control for the custom aspect ratio

        float m_rotationAngleRad = 0.0f; // Cumulative rotation progress in radians
    };

    DECLARE_DEMO(GrassDemo, "Grass Demo");
}

#endif // GRASSDEMO_H
