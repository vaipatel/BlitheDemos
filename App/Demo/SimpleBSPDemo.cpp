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
#include <tracy/Tracy.hpp>

namespace blithe
{
    ///
    /// \brief Destructor
    ///
    SimpleBSPDemo::~SimpleBSPDemo()
    {
        glDisable(GL_DEPTH_TEST);
        ClearAllBSPMeshObjects();
        delete m_shader;
        delete m_texture;
        delete m_cameraDecorator;
        delete m_bspTree;
        delete m_torus;
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

        // Switch between iterative batched nodes and full tree traversal.
        if ( m_dbgBatchedOrFullTraversal )
        {
            UpdateBSPTreeIterative(m_cameraDecorator->GetCamera());
        }
        else
        {
            UpdateBSPTreeFull(m_cameraDecorator->GetCamera());
        }

        m_shader->Bind();
        m_shader->SetUniformMat4f("viewProjection", viewProjection);

        // If we're doing iterative batched nodes traversal or view, show the wireframe while we're
        // traversing/view to mitigate crappy UX due to disappearing/missing triangles.
        if ( m_traversing || (m_maxDbgTrisPerRenderLoop > 0 && m_maxDbgTrisPerRenderLoop < m_bspNumTris) )
        {
            if ( m_torus )
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                m_torus->Render();
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

        size_t activeUnitOffset = 0;
        m_texture->Bind(activeUnitOffset);
        m_shader->SetUniform1i("myTex", static_cast<int>(activeUnitOffset));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //m_shader->SetUniformBool("useColorOverride", m_dbgBackToFrontWithGradient);
        if ( m_dbgBackToFrontWithGradient )
        {
            ReInitDbgVars();
            DrawDbgTris(deltaTimeS);
        }
        else
        {
            for ( size_t objIdx = 0; objIdx < m_bspMeshObjects.size(); objIdx++ )
            {
                std::vector<MeshObject*>& coplanarMeshObjects = m_bspMeshObjects[objIdx];
                for ( MeshObject* object : coplanarMeshObjects )
                {
                    object->Render();
                }
            }
        }

        glDisable(GL_BLEND);
        m_shader->Unbind();

        glDisable(GL_DEPTH_TEST);
    }

    void SimpleBSPDemo::OnDrawUI()
    {
        ImGui::Begin("SimpleBSPDemo Params");

        // Checkbox for switching between batched nodes tree traversal and full tree traversal
        bool dbgBatchedOrFullTraversal = m_dbgBatchedOrFullTraversal;
        ImGui::Checkbox("Do Batched Nodes Iterative Traversal", &dbgBatchedOrFullTraversal);
        if ( m_dbgBatchedOrFullTraversal != dbgBatchedOrFullTraversal )
        {
            m_dbgVarsValid = false;
            m_prevView = glm::mat4(0.0);
            m_dbgBatchedOrFullTraversal = dbgBatchedOrFullTraversal;
        }

        // Checkbox for debugging back-to-front polygon ordering with a gradient coloring
        bool dbgBackToFrontWithGradient = m_dbgBackToFrontWithGradient;
        ImGui::Checkbox("Color Back To Front", &dbgBackToFrontWithGradient);
        if ( m_dbgBackToFrontWithGradient != dbgBackToFrontWithGradient )
        {
            m_dbgVarsValid = false;
            m_dbgBackToFrontWithGradient = dbgBackToFrontWithGradient;
        }

        ImGui::End();
    }

    ///
    /// \brief Sets up the Torus mesh
    ///
    void SimpleBSPDemo::SetupTorus()
    {
        glm::mat4 modelTransform(1.0f);
        modelTransform = glm::rotate(modelTransform, glm::pi<float>()*0.5f, {1,0,0});
        Mesh torusMesh = CreateTorus(8, 2, 8, 8, {0.1, 0.5, 0.9, 0.1}, modelTransform);
        m_torus = new MeshObject(torusMesh);
        m_torus->SetInstances({glm::mat4(1.0f)});
    }

    ///
    /// \brief Sets up the meshes for the central cubes
    ///
    void SimpleBSPDemo::SetupCubes()
    {
        ASSERT(m_cubeMeshes.empty(), "Ensure m_cubes is empty before calling SetupCubes()");

        glm::vec3 sides(2,2,2);

        std::vector<glm::vec4> colors = {
            { 1.0f, 0.0f, 0.0f, 0.2f }, // red
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

    ///
    /// \brief Reinitializes the bsp tree and adds all mesh triangles to it.
    ///
    void SimpleBSPDemo::SetupBSPTree()
    {
        DeleteAndNull(m_bspTree);
        m_bspTree = new TriBSPTree();

        size_t origNumTris = 0;
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
            origNumTris += numTris;
        }

        if ( m_torus )
        {
            MeshView meshView(m_torus->GetMesh());
            size_t numTris = meshView.GetNumTriangles();
            std::vector<size_t> randomizedTriIndices = GetShuffledIndices(numTris);
            for ( size_t triIdx : randomizedTriIndices )
            {
                Tri tri = meshView.GetTriangle(triIdx);
                m_bspTree->AddTriangle(tri);
            }
            origNumTris += numTris;
        }

        TriBSPTree::CountTotalNumTris(m_bspTree, m_bspNumTris);
        std::cout << "\nSimpleBSPDemo: "
                  << "origNumTris = " << origNumTris << ", "
                  << "bspNumTris = " << m_bspNumTris << std::endl;
    }

    ///
    /// \brief Traverses the full BSP tree using the _camera position.
    ///
    ///        If there are many nodes, this will be very slow. The UpdateBSPTreeIterative() can be
    ///        called successively to traverse the tree a limited number of nodes at a time.
    ///
    /// \param _camera - Camera position to use for traversal
    ///
    void SimpleBSPDemo::UpdateBSPTreeFull(const Camera& _camera)
    {
        ZoneScoped;

        // Only traverse if the view has changed
        if ( _camera.GetViewMatrix() != m_prevView && m_bspTree )
        {
            m_prevView = _camera.GetViewMatrix();

            ClearAllBSPMeshObjects();

            // Traverse tree
            std::vector<std::vector<Tri>> trisList;
            TriBSPTree::TraverseRecursively(m_bspTree, _camera.GetPosition(), trisList);

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
                m_bspMeshObjects.push_back(meshObjects);
            }

            m_dbgVarsValid = false;
        }
    }

    ///
    /// \brief Traverses the BSP tree in batches of nodes using the _camera position.
    ///
    ///        Breaking up the traversal helps keep the framerate reasonable. The trade off is that
    ///        when the camera is moved, all the triangles disappear and start getting displayed
    ///        in batches, which makes for crappy UX. Maybe a base "skeleton" with few triangle or
    ///        lines can be rendered as the scene is being built so the user has a better sense of
    ///        their actions.
    ///
    /// \param _camera - Camera position to use for traversal
    ///
    void SimpleBSPDemo::UpdateBSPTreeIterative(const Camera& _camera)
    {
        ZoneScoped;

        // Only traverse if the view has changed
        if ( _camera.GetViewMatrix() != m_prevView && m_bspTree )
        {
            m_prevView = _camera.GetViewMatrix();

            ClearAllBSPMeshObjects();

            m_traversing = true;
            m_nodeStack = std::stack<TriBSPTreeStackEntry>();
        }

        if ( m_traversing )
        {
            // Traverse tree
            std::vector<std::vector<Tri>> trisList;
            TriBSPTree::TraverseWithStackIterative(m_bspTree,
                                                   _camera.GetPosition(),
                                                   trisList,
                                                   m_nodeStack,
                                                   24);
            m_traversing = !m_nodeStack.empty();

            for ( size_t i = 0; i < trisList.size(); i++ )
            {
                const std::vector<Tri>& coplanarTris = trisList[i];
                std::vector<MeshObject*> coplanarMeshObjects;
                for ( const Tri& tri : coplanarTris )
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
                    coplanarMeshObjects.push_back(object);
                }
                m_bspMeshObjects.push_back(coplanarMeshObjects);
            }

            m_dbgVarsValid = false;
        }
    }

    ///
    /// \brief Clears the list of all bsp mesh objects after deleting them.
    ///
    void SimpleBSPDemo::ClearAllBSPMeshObjects()
    {
        for ( size_t i = 0; i < m_bspMeshObjects.size(); i++ )
        {
            std::vector<MeshObject*>& coplanarMeshObjects = m_bspMeshObjects[i];
            for ( MeshObject* object : coplanarMeshObjects )
            {
                DeleteAndNull(object);
            }
        }
        m_bspMeshObjects.clear();
    }

    ///
    /// \brief If debug vars were marked invalid, this reinitializes them.
    ///
    void SimpleBSPDemo::ReInitDbgVars()
    {
        if ( !m_dbgVarsValid )
        {
            m_dbgVarsValid = true;
            m_dbgBackToFrontGradient = InterpolateColors({1,0,0,1.0},{1,1,1,0.1},m_bspMeshObjects.size());
            m_maxDbgTrisPerRenderLoop = 0;
            m_dbgTimeAccumulatorS = 0;
        }
    }

    ///
    /// \brief Animates the draw of the bsp traversed triangles to debug the back-to-front order.
    ///
    /// \param _deltaTimeS - (sec) Time since last frame
    ///
    void SimpleBSPDemo::DrawDbgTris(float _deltaTimeS)
    {
        ASSERT(m_dbgBackToFrontGradient.size() == m_bspMeshObjects.size(), "Gradient for debugging back to front polygon ordering is not populated correctly. Expected size " << m_bspMeshObjects.size() << ", got " << m_dbgBackToFrontGradient.size());

        m_dbgTimeAccumulatorS += _deltaTimeS;
        if ( m_dbgTimeAccumulatorS >= 0.01 )
        {
            m_dbgTimeAccumulatorS = 0;
            m_maxDbgTrisPerRenderLoop += 1;
        }

        size_t numTrisSoFar = 0;
        bool done = false;
        for ( size_t objIdx = 0; objIdx < m_bspMeshObjects.size() && !done; objIdx++ )
        {
            std::vector<MeshObject*>& coplanarMeshObjects = m_bspMeshObjects[objIdx];
            m_shader->SetUniformVec4f("colorOverride", m_dbgBackToFrontGradient[objIdx]);
            for ( size_t idx = 0; idx < coplanarMeshObjects.size() && !done; idx++ )
            {
                ++numTrisSoFar;
                done = (numTrisSoFar >= m_maxDbgTrisPerRenderLoop);
                MeshObject* object = coplanarMeshObjects[idx];
                object->Render();
            }
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
        if ( !_uiData.m_guiCaptured )
        {
            if ( _uiData.m_mouseMoved )
            {
                enCameraDraggedMouseBtn draggedBtn = _uiData.m_leftDragged ?
                                                         enCameraDraggedMouseBtn::LEFT :
                                                         enCameraDraggedMouseBtn::NONE;
                m_cameraDecorator->ProcessMouseMove(_uiData.m_xOffset, _uiData.m_yOffset,
                                                    draggedBtn, _deltaTime, true);
            }

            if ( _uiData.m_scrolled )
            {
                m_cameraDecorator->ProcessMouseScroll(_uiData.m_scrollX, _uiData.m_scrollY,
                                                      _deltaTime);
            }
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
}
