#include "InstancedCubeDemo.h"
#include "ArcBallCameraDecorator.h"
#include "BlithePath.h"
#include "GeomHelpers.h"
#include "MeshObject.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "UIData.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <ctime>

namespace blithe
{
    ///
    /// \brief Destructor
    ///
    InstancedCubeDemo::~InstancedCubeDemo()
    {
        glDisable(GL_DEPTH_TEST);
        delete m_cube;
        delete m_shader;
        delete m_texture;
        delete m_cameraDecorator;
    }

    void InstancedCubeDemo::OnInit()
    {
        std::string exePath = GetExecutablePath();
        m_texture = new Texture(exePath + "/Assets/vintage_convertible.jpg", TextureData::FilterParam::LINEAR);
        m_shader = new ShaderProgram(exePath + "/Shaders/TriangleInstanced.vert", exePath + "/Shaders/Triangle.frag");
        m_cameraDecorator = new ArcBallCameraDecorator();

        SetupCube();
    }

    void InstancedCubeDemo::OnRender(double _deltaTimeS, const UIData& _uiData)
    {
        float deltaTimeS = static_cast<float>(_deltaTimeS);

        glEnable(GL_DEPTH_TEST);
        ImVec4 clearCol = _uiData.m_clearColor;
        glClearColor(clearCol.x * clearCol.w, clearCol.y * clearCol.w, clearCol.z * clearCol.w, clearCol.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const float pi = glm::pi<float>();

        ProcessKeys(_uiData, deltaTimeS);
        ProcessMouseMove(_uiData, deltaTimeS);

        m_rotationAngleRad += deltaTimeS * m_rotationSpeed * 2.0f * pi; // Update based on speed
        m_rotationAngleRad = fmod(m_rotationAngleRad, 2.0f * pi); // Keep progress within a full rotation range

        // create transformations (inspired from learnopengl.com)
        //glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        //model = glm::rotate(model, m_rotationAngleRad, glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(0.5f));
        glm::mat4 view = m_cameraDecorator->GetCamera().GetViewMatrix();
        float aspect = m_useCustomAspect ? m_customAspect : _uiData.m_aspect;
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        glm::mat4 viewProjection = projection * view; // I just mat mult here as opposed to per vertex in the shader

        m_shader->Bind();
        m_shader->SetUniformMat4f("viewProjection", viewProjection);

        size_t activeUnitOffset = 0;
        m_texture->Bind(activeUnitOffset);
        m_shader->SetUniform1i("myTex", static_cast<int>(activeUnitOffset));

        m_cube->Render();

        m_shader->Unbind();

        glDisable(GL_DEPTH_TEST);
    }

    void InstancedCubeDemo::OnDrawUI()
    {
        ImGui::Begin("Instanced Cube Demo Params");

        // Slider for Rotation Speed
        ImGui::SliderFloat("Rotation Speed", &m_rotationSpeed, 0.0f, 1.0f);

        // Checkbox for enabling/disabling custom aspect ratio
        ImGui::Checkbox("Custom Aspect", &m_useCustomAspect);

        // Disable the slider if the checkbox is not checked
        if (!m_useCustomAspect) {
            ImGui::BeginDisabled();
        }

        // Slider for custom aspect ratio
        ImGui::SliderFloat("Aspect Ratio", &m_customAspect, 0.1f, 4.0f, "%.2f");

        // End disabled state if it was begun
        if (!m_useCustomAspect) {
            ImGui::EndDisabled();
        }

        ImGui::End();
    }

    void InstancedCubeDemo::SetupCube()
    {
        glm::vec3 sides(1,1,1);

        std::vector<glm::vec4> colors = {
            { 1.0f, 0.0f, 0.0f, 1.0f }, // red
            { 0.0f, 1.0f, 0.0f, 1.0f }, // green
            { 0.0f, 0.0f, 1.0f, 1.0f }, // blue
            { 1.0f, 1.0f, 0.0f, 1.0f }, // yellow
            { 1.0f, 0.0f, 1.0f, 1.0f }, // magenta
            { 0.0f, 1.0f, 1.0f, 1.0f }, // cyan
            { 1.0f, 0.5f, 0.0f, 1.0f }, // orange
            { 0.5f, 0.0f, 0.5f, 1.0f }, // purple
        };

        Mesh mesh = GeomHelpers::CreateCuboid(sides, colors);

        m_cube = new MeshObject(mesh);

        std::vector<glm::mat4> modelTransforms = GenerateGridModelMatrices(10, 10, 10, 2.0);
        m_cube->SetInstances(modelTransforms);
    }

    void InstancedCubeDemo::ProcessKeys(const UIData& _uiData, float _deltaTime)
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

    void InstancedCubeDemo::ProcessMouseMove(const UIData& _uiData, float _deltaTime)
    {
        if ( !_uiData.m_guiCaptured )
        {
            const MouseInputState& mouseInput = _uiData.m_mouseInput;
            if ( mouseInput.m_mouseMoved )
            {
                m_cameraDecorator->ProcessMouseMove(mouseInput.m_mouseDelta.x,
                                                    mouseInput.m_mouseDelta.y,
                                                    mouseInput.GetDraggedButtons(),
                                                    _deltaTime,
                                                    true);
            }
        }
    }

    ///
    /// \brief Generate a vector of model matrices with random transformations
    ///        (source: chatgippity)
    ///
    /// \param _count          - Number of model matrices to generate
    /// \param _maxTranslation - Maximum range for translation values (applied in each direction)
    /// \param _minScale       - Minimum scaling factor
    /// \param _maxScale       - Maximum scaling factor
    ///
    /// \return A vector of glm::mat4 model matrices with randomized transformations
    ///
    std::vector<glm::mat4> InstancedCubeDemo::GenerateRandomModelMatrices(int _count, float _maxTranslation, float _minScale, float _maxScale)
    {
        std::vector<glm::mat4> modelMatrices;
        modelMatrices.reserve(_count);

        // Initialize random seed
        std::srand(static_cast<unsigned>(std::time(nullptr)));

        for (int i = 0; i < _count; ++i)
        {
            // Translation
            float transX = _maxTranslation * ((std::rand() / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f);
            float transY = _maxTranslation * ((std::rand() / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f);
            float transZ = _maxTranslation * ((std::rand() / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f);

            // Scale
            float scale = _minScale + (std::rand() / static_cast<float>(RAND_MAX)) * (_maxScale - _minScale);

            // Rotation
            float angleX = static_cast<float>(std::rand() % 360);
            float angleY = static_cast<float>(std::rand() % 360);
            float angleZ = static_cast<float>(std::rand() % 360);

            // Create model matrix
            glm::mat4 model = glm::mat4(1.0f);

            // Apply translation
            model = glm::translate(model, glm::vec3(transX, transY, transZ));

            // Apply rotation
            model = glm::rotate(model, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(angleZ), glm::vec3(0.0f, 0.0f, 1.0f));

            // Apply scaling
            model = glm::scale(model, glm::vec3(scale));

            modelMatrices.push_back(model);
        }

        return modelMatrices;
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
    std::vector<glm::mat4> InstancedCubeDemo::GenerateGridModelMatrices(int _xCount, int _yCount, int _zCount, float _spacing)
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
}
