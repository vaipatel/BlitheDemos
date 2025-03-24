#include "CubeDemo.h"
#include "ArcBallCameraDecorator.h"
#include "BlithePath.h"
#include "CachedMeshObject.h"
#include "GeomHelpers.h"
#include "GLBufferCache.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "UIData.h"

#include "imgui.h"
#include <stdio.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <tracy/Tracy.hpp>

namespace blithe
{
    /*!
     * \brief Destructor
     */
    CubeDemo::~CubeDemo()
    {
        glDisable(GL_DEPTH_TEST);
        delete m_cachedCube;
        m_glBufferCache->CleanUp();
        delete m_glBufferCache;
        delete m_cubeMesh;
        delete m_shader;
        delete m_texture;
        delete m_cameraDecorator;
    }

    /*!
     * \brief Setup for the cube demo
     */
    void CubeDemo::OnInit()
    {
        std::string exePath = GetExecutablePath();
        m_texture = new Texture(exePath + "/Assets/vintage_convertible.jpg", TextureData::FilterParam::LINEAR);
        m_shader = new ShaderProgram(exePath + "/Shaders/Triangle.vert", exePath + "/Shaders/Triangle.frag");
        m_cameraDecorator = new ArcBallCameraDecorator();

        SetupCubeMesh();
        m_glBufferCache = new GLBufferCache();
    }

    /*!
     * \brief Render the cube
     */
    void CubeDemo::OnRender(double _deltaTimeS, const UIData& _uiData)
    {
        ZoneScopedN("CubeDemo::OnRender");

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
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        model = glm::rotate(model, m_rotationAngleRad, glm::vec3(0.5f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f));
        view = m_cameraDecorator->GetCamera().GetViewMatrix(); //glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f));
        float aspect = m_useCustomAspect ? m_customAspect : _uiData.m_aspect;
        projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        glm::mat4 matrix = projection * view * model; // I just mat mult here as opposed to per vertex in the shader

        m_shader->Bind();
        m_shader->SetUniformMat4f("matrix", matrix);

        size_t activeUnitOffset = 0;
        m_texture->Bind(activeUnitOffset);
        m_shader->SetUniform1i("myTex", static_cast<int>(activeUnitOffset));

        {
            ZoneScopedN("Cube");

            m_cachedCube = new CachedMeshObject("Cube", m_cubeMesh, m_glBufferCache);
            m_cachedCube->Render();
            delete m_cachedCube;
            m_cachedCube = nullptr;
        }

        m_shader->Unbind();

        glDisable(GL_DEPTH_TEST);
    }

    /*!
     * \brief ImGui controls specific to the cube demo
     */
    void CubeDemo::OnDrawUI()
    {
        ImGui::Begin("Cube Demo Params");

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

    ///
    /// \brief Sets up the cube mesh data
    ///
    void CubeDemo::SetupCubeMesh()
    {
        glm::vec3 sides(2,2,2);

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
        m_cubeMesh = new Mesh(mesh);
    }

    void CubeDemo::ProcessKeys(const UIData& _uiData, float _deltaTime)
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

    void CubeDemo::ProcessMouseMove(const UIData& _uiData, float _deltaTime)
    {
        if ( _uiData.m_mouseMoved && !_uiData.m_guiCaptured )
        {
            m_cameraDecorator->ProcessMouseMove(_uiData.m_xOffset, _uiData.m_yOffset,
                                                _uiData.m_leftDragged, _deltaTime, true);
        }
    }
}
