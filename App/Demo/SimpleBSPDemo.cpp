#include "SimpleBSPDemo.h"
#include "ArcBallCameraDecorator.h"
#include "BlithePath.h"
#include "BlitheShared.h"
#include "GeomHelpers.h"
#include "MeshView.h"
#include "TriBSPTree.h"
#include "MeshObject.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "UIData.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>           // Includes the data structure
#include <assimp/postprocess.h>     // Includes the post processing flags

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <ctime>
#include <algorithm>
#include <random>

namespace blithe
{
    ///
    /// \brief Destructor
    ///
    SimpleBSPDemo::~SimpleBSPDemo()
    {
        glDisable(GL_DEPTH_TEST);
        for ( MeshObject* cube : m_cubes)
        {
            delete cube;
        }
        m_cubes.clear();
        delete m_shader;
        delete m_texture;
        delete m_cameraDecorator;
        delete m_bspTree;
    }

    void SimpleBSPDemo::OnInit()
    {
        std::string exePath = GetExecutablePath();
        m_texture = new Texture(exePath + "/Assets/vintage_convertible.jpg", TextureData::FilterParam::LINEAR);
        m_shader = new ShaderProgram(exePath + "/Shaders/TriangleInstanced.vert", exePath + "/Shaders/Triangle.frag");
        m_cameraDecorator = new ArcBallCameraDecorator({0,10,20});

        SetupTorus();
        SetupCubes();
        SetupBSPTree();
    }

    void SimpleBSPDemo::OnRender(double _deltaTimeS, const UIData& _uiData)
    {
        float deltaTimeS = static_cast<float>(_deltaTimeS);

        // glEnable(GL_DEPTH_TEST);
        ImVec4 clearCol = {0, 0.1f, 0.1f, 1};//_uiData.m_clearColor;
        glClearColor(clearCol.x * clearCol.w, clearCol.y * clearCol.w, clearCol.z * clearCol.w, clearCol.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ProcessKeys(_uiData, deltaTimeS);
        ProcessMouseMove(_uiData, deltaTimeS);

        glm::mat4 view = m_cameraDecorator->GetCamera().GetViewMatrix();
        float aspect = _uiData.m_aspect;
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        glm::mat4 viewProjection = projection * view;

        UpdateBSPTree(m_cameraDecorator->GetCamera());

        m_shader->Bind();
        m_shader->SetUniformMat4f("viewProjection", viewProjection);

        size_t activeUnitOffset = 0;
        m_texture->Bind(activeUnitOffset);
        m_shader->SetUniform1i("myTex", static_cast<int>(activeUnitOffset));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_shader->SetUniformBool("useColorOverride", m_dbgBackToFrontWithGradient);
        ASSERT(m_dbgBackToFrontGradient.size() == m_meshObjects.size(), "Gradient for debugging back to front polygon ordering is not populated correctly. Expected size " << m_meshObjects.size() << ", got " << m_dbgBackToFrontGradient.size());

        for ( size_t objIdx = 0; objIdx < m_meshObjects.size(); objIdx++ )
        {
            std::vector<MeshObject*>& meshObjects = m_meshObjects[objIdx];
            m_shader->SetUniformVec4f("colorOverride", m_dbgBackToFrontGradient[objIdx]);
            for ( MeshObject* object : meshObjects )
            {
                object->Render();
            }
        }
        glDisable(GL_BLEND);
        m_shader->Unbind();

        glDisable(GL_DEPTH_TEST);
    }

    void SimpleBSPDemo::OnDrawUI()
    {
        ImGui::Begin("SimpleBSPDemo Params");

        // Checkbox for debugging back-to-front polygon ordering with a gradient coloring
        ImGui::Checkbox("Color Back To Front", &m_dbgBackToFrontWithGradient);

        ImGui::End();
    }

    void SimpleBSPDemo::SetupTorus()
    {
        glm::mat4 modelTransform(1.0f);
        modelTransform = glm::rotate(modelTransform, glm::pi<float>()*0.5f, {1,0,0});
        Mesh torusMesh = CreateTorus(8, 2, 8, 8, {0.1, 0.5, 0.9, 0.1}, modelTransform);
        m_torus = new MeshObject(torusMesh);
        m_torus->SetInstances({glm::mat4(1.0f)});
    }

    void SimpleBSPDemo::SetupCubes()
    {
        glm::vec3 sides(2,2,2);

        std::vector<glm::vec4> colors = {
            { 1.0f, 0.0f, 0.0f, 0.4f }, // red
            { 0.0f, 1.0f, 0.0f, 0.4f }, // green
            { 0.0f, 0.0f, 1.0f, 0.4f }, // blue
            { 1.0f, 1.0f, 0.0f, 0.4f }, // yellow
            { 1.0f, 0.0f, 1.0f, 0.4f }, // magenta
            { 0.0f, 1.0f, 1.0f, 0.4f }, // cyan
            { 1.0f, 0.5f, 0.0f, 0.4f }, // orange
            { 0.5f, 0.0f, 0.5f, 0.4f }, // purple
        };

        std::vector<glm::mat4> modelTransforms = GenerateGridModelMatrices(2, 2, 2, 3.0);
        ASSERT(modelTransforms.size() > 0, "There must be atleast 1 modelTransform");

        for ( size_t i = 0; i < modelTransforms.size(); i++ )
        {
            glm::vec4 chosenColor = colors[i%colors.size()];
            std::vector<glm::vec4> chosenColorVec(colors.size(), chosenColor);
            Mesh mesh = GeomHelpers::CreateCuboid(sides, chosenColorVec, modelTransforms[i]);
            m_cubeMeshes.push_back(mesh);
        }
    }

    void SimpleBSPDemo::SetupBSPTree()
    {
        DeleteAndNull(m_bspTree);
        m_bspTree = new TriBSPTree();
        for ( size_t i = 0; i < m_cubeMeshes.size(); i++ )
        {
            Mesh mesh = m_cubeMeshes[i];
            MeshView meshView(mesh);
            size_t numTris = meshView.GetNumTriangles();
            std::vector<size_t> randomizedTriIndices = GetShuffledIndices(numTris);
            for ( size_t triIdx : randomizedTriIndices )
            {
                Tri tri = meshView.GetTriangle(triIdx);
                m_bspTree->AddTriangle(tri);
            }
        }

        {
            const Mesh& mesh = m_torus->GetMesh();
            MeshView meshView(mesh);
            size_t numTris = meshView.GetNumTriangles();
            std::vector<size_t> randomizedTriIndices = GetShuffledIndices(numTris);
            for ( size_t triIdx : randomizedTriIndices )
            {
                Tri tri = meshView.GetTriangle(triIdx);
                m_bspTree->AddTriangle(tri);
            }
        }
    }

    void SimpleBSPDemo::UpdateBSPTree(const Camera& _camera)
    {
        if ( _camera.GetViewMatrix() != m_prevView && m_bspTree )
        {
            m_prevView = _camera.GetViewMatrix();

            for ( size_t i = 0; i < m_meshObjects.size(); i++ )
            {
                std::vector<MeshObject*>& meshObjects = m_meshObjects[i];
                for ( MeshObject* object : meshObjects )
                {
                    DeleteAndNull(object);
                }
            }
            m_meshObjects.clear();

            // Traverse tree
            std::vector<std::vector<Tri>> trisList;
            TriBSPTree::Traverse(m_bspTree, _camera.GetPosition(), trisList);

            for ( size_t i = 0; i < trisList.size(); i++ )
            {
                const std::vector<Tri>& tris = trisList[i];
                std::vector<MeshObject*> meshObjects;
                for ( const Tri& tri : tris )
                {
                    Mesh mesh;
                    mesh.m_vertices.push_back(tri.m_v0);
                    mesh.m_vertices.push_back(tri.m_v1);
                    mesh.m_vertices.push_back(tri.m_v2);
                    mesh.m_indices.push_back(0);
                    mesh.m_indices.push_back(1);
                    mesh.m_indices.push_back(2);
                    MeshObject* object = new MeshObject(mesh);
                    object->SetInstances({glm::mat4(1.0f)});
                    meshObjects.push_back(object);
                }
                m_meshObjects.push_back(meshObjects);
            }

            m_dbgBackToFrontGradient = InterpolateColors({1,0,0,1.0},{1,1,1,0.1},m_meshObjects.size());
        }
    }


    void SimpleBSPDemo::ProcessKeys(const UIData& _uiData, float _deltaTime)
    {
        if ( _uiData.m_pressedKeys.count(enPressedKey::KEY_W) > 0 )
        {
            m_cameraDecorator->ProcessKeyboard(enCameraMovement::FORWARD, _deltaTime);
        }

        if ( _uiData.m_pressedKeys.count(enPressedKey::KEY_S) > 0 )
        {
            m_cameraDecorator->ProcessKeyboard(enCameraMovement::BACKWARD, _deltaTime);
        }

        if ( _uiData.m_pressedKeys.count(enPressedKey::KEY_A) > 0 )
        {
            m_cameraDecorator->ProcessKeyboard(enCameraMovement::LEFT, _deltaTime);
        }

        if ( _uiData.m_pressedKeys.count(enPressedKey::KEY_D) > 0 )
        {
            m_cameraDecorator->ProcessKeyboard(enCameraMovement::RIGHT, _deltaTime);
        }
    }

    void SimpleBSPDemo::ProcessMouseMove(const UIData& _uiData, float _deltaTime)
    {
        if ( _uiData.m_mouseMoved && !_uiData.m_guiCaptured )
        {
            m_cameraDecorator->ProcessMouseMove(_uiData.m_xOffset, _uiData.m_yOffset,
                                                _uiData.m_leftDragged, _deltaTime, true);
        }
    }

    ///
    /// \brief Generate a vector of model matrices arranged in a 3D grid
    ///        (source: chatgippity)
    ///
    /// \param _xCount  - Number of cubes in the X direction
    /// \param _yCount  - Number of cubes in the Y direction
    /// \param _zCount  - Number of cubes in the Z direction
    /// \param _spacing - Distance between cubes along each axis
    ///
    /// \return A vector of glm::mat4 model matrices for each cube in the grid
    ///
    std::vector<glm::mat4> SimpleBSPDemo::GenerateGridModelMatrices(int _xCount, int _yCount, int _zCount, float _spacing)
    {
        std::vector<glm::mat4> modelMatrices;
        modelMatrices.reserve(_xCount * _yCount * _zCount);

        for (int x = 0; x < _xCount; ++x)
        {
            for (int y = 0; y < _yCount; ++y)
            {
                for (int z = 0; z < _zCount; ++z)
                {
                    glm::mat4 model = glm::mat4(1.0f);

                    // Calculate position in the grid based on spacing
                    float posX = (x - (_xCount - 1) / 2.0f) * _spacing;
                    float posY = (y - (_yCount - 1) / 2.0f) * _spacing;
                    float posZ = (z - (_zCount - 1) / 2.0f) * _spacing;

                    // Apply translation to the model matrix
                    model = glm::translate(model, glm::vec3(posX, posY, posZ));

                    // Add the model matrix to the list
                    modelMatrices.push_back(model);
                }
            }
        }

        return modelMatrices;
    }

    ///
    /// \brief Creates a torus mesh. (source: chatgippity)
    ///
    /// \param _majorRadius    - Major radius R, the distance from the center of the torus to the center of the tube
    /// \param _minorRadius    - Minor radius r, the radius of the tube itself
    /// \param _numSides       - the number of subdivisions around the torus's central ring
    /// \param _numRings       - the number of subdivisions around the torus's tube
    /// \param _color          - Color to use for the vertices
    /// \param _modelTransform - (optional) Transform to apply to all vertex positions.
    ///                          Defaults to identity.
    ///
    /// \return Torus mesh
    ///
    Mesh SimpleBSPDemo::CreateTorus(float _majorRadius, float _minorRadius,
                                    int _numSides, int _numRings,
                                    const glm::vec4& _color,
                                    const glm::mat4& _modelTransform/* = glm::mat4(1.0f)*/)
    {
        Mesh torusMesh;

        float sideStep = glm::two_pi<float>() / static_cast<float>(_numSides);  // Angle step around minor radius
        float ringStep = glm::two_pi<float>() / static_cast<float>(_numRings);  // Angle step around major radius

        // Generate vertices
        for (int ring = 0; ring <= _numRings; ++ring)
        {
            float phi = static_cast<float>(ring) * ringStep;  // Angle along major radius
            glm::vec3 ringCenter = glm::vec3(_majorRadius * glm::cos(phi), _majorRadius * glm::sin(phi), 0.0f);

            for (int side = 0; side <= _numSides; ++side)
            {
                float theta = static_cast<float>(side) * sideStep;  // Angle along tube
                glm::vec3 position = ringCenter +
                                     glm::vec3(_minorRadius * glm::cos(theta) * glm::cos(phi),
                                               _minorRadius * glm::cos(theta) * glm::sin(phi),
                                               _minorRadius * glm::sin(theta));

                // Texture coordinates for wrapping
                glm::vec2 texCoords = glm::vec2(static_cast<float>(ring) / static_cast<float>(_numRings),
                                                static_cast<float>(side) / static_cast<float>(_numSides));

                // Add vertex to list
                position = _modelTransform * glm::vec4(position, 1.0f);
                torusMesh.m_vertices.push_back({ position, _color, texCoords });
            }
        }

        // Generate indices for triangle strips
        for (int ring = 0; ring < _numRings; ++ring)
        {
            for (int side = 0; side < _numSides; ++side)
            {
                unsigned int current = static_cast<unsigned int>(ring * (_numSides + 1) + side);
                unsigned int next = current + static_cast<unsigned int>(_numSides) + 1;

                torusMesh.m_indices.push_back(current);
                torusMesh.m_indices.push_back(next);
                torusMesh.m_indices.push_back(current + 1);

                torusMesh.m_indices.push_back(current + 1);
                torusMesh.m_indices.push_back(next);
                torusMesh.m_indices.push_back(next + 1);
            }
        }

        return torusMesh;
    }

    ///
    /// \brief Interpolates between two glm::vec4 colors over a specified number of steps.
    ///
    /// \param _startColor - The starting color as a glm::vec4.
    /// \param _endColor   - The ending color as a glm::vec4.
    /// \param _steps      - The number of steps for interpolation. Should be >= 2 for meaningful results.
    ///
    /// \return A vector containing the interpolated colors as glm::vec4.
    ///
    std::vector<glm::vec4> SimpleBSPDemo::InterpolateColors(const glm::vec4& _startColor, const glm::vec4& _endColor, size_t _steps)
    {
        // Ensure at least 2 steps for interpolation
        if (_steps < 2)
        {
            return {_startColor, _endColor};
        }

        std::vector<glm::vec4> colors;
        colors.reserve(_steps);

        // Calculate the interpolation step size
        float stepSize = 1.0f / static_cast<float>(_steps - 1);

        for (size_t i = 0; i < _steps; ++i)
        {
            // Interpolation factor between 0 and 1
            float t = static_cast<float>(i) * stepSize;

            // Linearly interpolate each component
            glm::vec4 color = glm::mix(_startColor, _endColor, t);

            colors.push_back(color);
        }

        return colors;
    }

    std::vector<size_t> SimpleBSPDemo::GetShuffledIndices(size_t _numItems)
    {
        std::random_device rd;
        std::mt19937 generator(rd());

        // Create a vector of indices
        std::vector<size_t> indices(_numItems);
        for (size_t i = 0; i < indices.size(); ++i)
        {
            indices[i] = i;
        }

        // Shuffle indices
        std::shuffle(indices.begin(), indices.end(), generator);

        return indices;
    }

    ///
    /// \brief Factory destructor
    ///
    SimpleBSPDemoFactory::~SimpleBSPDemoFactory()
    {
    }

    ///
    /// \brief Gets the name of the SimpleBSPDemo
    ///
    /// \return Name of the SimpleBSPDemo
    ///
    std::string SimpleBSPDemoFactory::GetName() const
    {
        return "Simple BSP Demo";
    }

    ///
    /// \brief Creates the SimpleBSPDemo
    ///
    /// \return A new SimpleBSPDemo
    ///
    DemoInterface* SimpleBSPDemoFactory::CreateDemo()
    {
        return new SimpleBSPDemo();
    }
}
