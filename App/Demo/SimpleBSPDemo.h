#ifndef SIMPLEBSPDEMO_H
#define SIMPLEBSPDEMO_H

#include <stack>
#include <vector>
#include <glm/glm.hpp>
#include "DemoInterface.h"
#include "Mesh.h"
#include "TriBSPTree.h"

namespace blithe
{
    class Camera;
    class CameraDecorator;
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
        void UpdateBSPTreeFull(const Camera& _camera);
        void UpdateBSPTreeIterative(const Camera& _camera);
        void ClearAllBSPMeshObjects();
        void ReInitDbgVars();
        void DrawDbgTris(float _deltaTimeS);
        void ProcessKeys(const UIData& _uiData, float _deltaTime);
        void ProcessMouseMove(const UIData& _uiData, float _deltaTime);
        std::vector<glm::mat4> GenerateGridModelMatrices(int _xCount, int _yCount, int _zCount, float _spacing);
        static Mesh CreateTorus(float _majorRadius, float _minorRadius,
                                int _numSides, int _numRings, const glm::vec4& _color,
                                const glm::mat4& _modelTransform = glm::mat4(1.0f));

        std::vector<glm::vec4> InterpolateColors(const glm::vec4& _startColor, const glm::vec4& _endColor, size_t _steps);
        std::vector<size_t> GetShuffledIndices(size_t _numItems);

        CameraDecorator* m_cameraDecorator = nullptr; //!< Arc ball camera decorator
        ShaderProgram* m_shader = nullptr; //!< Simple triangle shader to use for all the triangles
        Texture* m_texture = nullptr;      //!< Simple texture to use for all the object "faces"
        std::vector<Mesh> m_cubeMeshes;    //!< List of meshes for the central cubes
        MeshObject* m_cubes = nullptr;     //!< Cube objects for central cubes (we make a renderable object so we can wireframe it)
        MeshObject* m_torus = nullptr;     //!< Torus mesh object (we make a renderable object so we can wireframe it)

        TriBSPTree* m_bspTree = nullptr; //!< BSP Tree. All mesh tris are added to it on Setup. It is traversed in the Render loop.
        std::vector<std::vector<MeshObject*>> m_bspMeshObjects; //!< List of list of coplanar mesh objects obtained from bsp traversal, so they should be in back to front order.
        size_t m_bspNumTris = 0; //!< Num tris in bsp tree
        glm::mat4 m_prevView = glm::mat4(0.0f); //!< View matrix "key" to cache the traversal
        bool m_traversing = false; //!< Whether we're currently doing the iterative traverse
        std::stack<TriBSPTreeStackEntry> m_nodeStack; //!< Stack passed to iterative traversal
        bool m_dbgBatchedOrFullTraversal = true; //!< Value from UI for whether were should be doing iterative (batched nodes) tree traversal or full tree traversal.

        bool m_dbgBackToFrontWithGradient = false; //!< Value from UI control for whether we should use a gradient of colors to color polygons from back to front, for debugging
        std::vector<glm::vec4> m_dbgBackToFrontGradient; //!< Gradient of colors to color polygons from back to front, for debugging
        size_t m_maxDbgTrisPerRenderLoop = 0; //!< Num tris to add to render per loop. For debugging back to front order.
        float m_dbgTimeAccumulatorS = 0; //!< "Animation" time period accumulator. For debugging back to front order.
        bool m_dbgVarsValid = false; //!< For re-initializing the "dbg" vars
    };

    DECLARE_DEMO(SimpleBSPDemo, "Simple BSP Demo");
}

#endif // SIMPLEBSPDEMO_H
