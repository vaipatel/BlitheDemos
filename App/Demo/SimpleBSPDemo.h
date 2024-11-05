#ifndef SIMPLEBSPDEMO_H
#define SIMPLEBSPDEMO_H

#include <vector>
#include <glm/glm.hpp>
#include "DemoInterface.h"
#include "Mesh.h"

namespace blithe
{
    class Camera;
    class CameraDecorator;
    class TriBSPTree;
    class MeshObject;
    class ShaderProgram;
    class Texture;
    class TrisObject;

    class SimpleBSPDemo : public DemoInterface
    {
    public:
        ~SimpleBSPDemo() override;

        void OnInit() override;

        void OnRender(double _deltaTimeS, const UIData& _uiData) override;

        void OnDrawUI() override;

        bool UsesStandardViewPort() const override { return true; }

    private:
        void SetupTorus();
        void SetupCubes();
        void SetupBSPTree();
        void UpdateBSPTree(const Camera& _camera);
        void ProcessKeys(const UIData& _uiData, float _deltaTime);
        void ProcessMouseMove(const UIData& _uiData, float _deltaTime);
        std::vector<glm::mat4> GenerateGridModelMatrices(int _xCount, int _yCount, int _zCount, float _spacing);
        static Mesh CreateTorus(float _majorRadius, float _minorRadius,
                                int _numSides, int _numRings, const glm::vec4& _color,
                                const glm::mat4& _modelTransform = glm::mat4(1.0f));

        std::vector<glm::vec4> InterpolateColors(const glm::vec4& _startColor, const glm::vec4& _endColor, size_t _steps);
        std::vector<size_t> GetShuffledIndices(size_t _numItems);

        ShaderProgram* m_shader = nullptr;
        std::vector<Mesh> m_cubeMeshes;
        TriBSPTree* m_bspTree = nullptr;
        MeshObject* m_torus = nullptr;
        std::vector<MeshObject*> m_cubes;
        Texture* m_texture = nullptr;
        std::vector<std::vector<MeshObject*>> m_meshObjects;
        CameraDecorator* m_cameraDecorator = nullptr;
        bool m_dbgBackToFrontWithGradient = false; //!< Value from UI control for whether we should use a gradient of colors to color polygons from back to front, for debugging
        std::vector<glm::vec4> m_dbgBackToFrontGradient;

        glm::mat4 m_prevView = glm::mat4(0.0f);
    };

    class SimpleBSPDemoFactory : public DemoFactory
    {
    public:
        ~SimpleBSPDemoFactory() override;
        std::string GetName() const override;
        DemoInterface* CreateDemo() override;
    };
}

#endif // SIMPLEBSPDEMO_H
