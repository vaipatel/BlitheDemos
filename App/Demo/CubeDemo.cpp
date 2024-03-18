#include "CubeDemo.h"
#include "ImPath.h"
#include "MeshObject.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "UIData.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

namespace IME
{
    /*!
     * \brief Destructor
     */
    CubeDemo::~CubeDemo()
    {
        glDisable(GL_DEPTH_TEST);
        delete m_cube;
        delete m_shader;
        delete m_texture;
    }

    /*!
     * \brief Setup for the cube demo
     */
    void CubeDemo::OnInit()
    {
        std::string exePath = GetExecutablePath();
        m_texture = new Texture(exePath + "/Assets/vintage_convertible.jpg", TextureData::FilterParam::LINEAR);
        m_shader = new ShaderProgram(exePath + "/Shaders/Triangle.vert", exePath + "/Shaders/Triangle.frag");
        SetupCube();

        m_lastFrameTime = glfwGetTime();
    }

    /*!
     * \brief Render the cube
     */
    void CubeDemo::OnRender(const UIData& _uiData)
    {
        glEnable(GL_DEPTH_TEST);
        glClearColor(_uiData.m_clearColor.x * _uiData.m_clearColor.w, _uiData.m_clearColor.y * _uiData.m_clearColor.w, _uiData.m_clearColor.z * _uiData.m_clearColor.w, _uiData.m_clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const float pi = glm::pi<float>();

        float currentTime = glfwGetTime(); // Get elapsed time since GLFW started
        float deltaTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;

        m_rotationAngleRad += deltaTime * m_rotationSpeed * 2.0f * pi; // Update based on speed
        m_rotationAngleRad = fmod(m_rotationAngleRad, 2.0f * pi); // Keep progress within a full rotation range

        // create transformations (inspired from learnopengl.com)
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        model = glm::rotate(model, m_rotationAngleRad, glm::vec3(0.5f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f));
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f));
        float aspect = m_useCustomAspect ? m_customAspect : _uiData.m_aspect;
        projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        glm::mat4 matrix = projection * view * model; // I just mat mult here as opposed to per vertex in the shader

        m_shader->Bind();
        m_shader->SetUniformMat4f("matrix", matrix);

        m_cube->Render();

        m_shader->Unbind();

        glDisable(GL_DEPTH_TEST);
    }

    /*!
     * \brief ImGui controls specific to the cube demo
     */
    void CubeDemo::OnImGuiRender()
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

    /*!
     * \brief Helper to setup the cube
     */
    void CubeDemo::SetupCube()
    {
        // Define the vertices of the cube
        std::vector<Vertex> vertices = {
            // Front face
            { { -1.0f, -1.0f,  1.0f },{ 1.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f } }, // Bottom-left
            { { 1.0f, -1.0f,  1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 0.0f } }, // Bottom-right
            { { 1.0f,  1.0f,  1.0f },{ 0.0f, 0.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } }, // Top-right
            { { -1.0f,  1.0f,  1.0f },{ 1.0f, 1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } }, // Top-left

                                                                                   // Back face
            { { -1.0f, -1.0f, -1.0f },{ 1.0f, 0.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } }, // Bottom-left
            { { 1.0f, -1.0f, -1.0f },{ 0.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } }, // Bottom-right
            { { 1.0f,  1.0f, -1.0f },{ 1.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } }, // Top-right
            { { -1.0f,  1.0f, -1.0f },{ 0.0f, 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } }, // Top-left

                                                                                   // Left face
            { { -1.0f, -1.0f, -1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } }, // Bottom-left
            { { -1.0f, -1.0f,  1.0f },{ 0.5f, 0.0f, 0.5f, 1.0f },{ 1.0f, 0.0f } }, // Bottom-right
            { { -1.0f,  1.0f,  1.0f },{ 0.5f, 0.5f, 0.0f, 1.0f },{ 1.0f, 1.0f } }, // Top-right
            { { -1.0f,  1.0f, -1.0f },{ 0.0f, 0.5f, 0.5f, 1.0f },{ 0.0f, 1.0f } }, // Top-left

                                                                                   // Right face
            { { 1.0f, -1.0f,  1.0f },{ 0.5f, 0.5f, 0.5f, 1.0f },{ 0.0f, 0.0f } }, // Bottom-left
            { { 1.0f, -1.0f, -1.0f },{ 1.0f, 0.5f, 0.0f, 1.0f },{ 1.0f, 0.0f } }, // Bottom-right
            { { 1.0f,  1.0f, -1.0f },{ 0.0f, 1.0f, 0.5f, 1.0f },{ 1.0f, 1.0f } }, // Top-right
            { { 1.0f,  1.0f,  1.0f },{ 0.5f, 0.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } }, // Top-left

                                                                                  // Top face
            { { -1.0f,  1.0f,  1.0f },{ 0.0f, 0.5f, 1.0f, 1.0f },{ 0.0f, 0.0f } }, // Bottom-left
            { { 1.0f,  1.0f,  1.0f },{ 1.0f, 0.0f, 0.5f, 1.0f },{ 1.0f, 0.0f } }, // Bottom-right
            { { 1.0f,  1.0f, -1.0f },{ 0.5f, 1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } }, // Top-right
            { { -1.0f,  1.0f, -1.0f },{ 1.0f, 0.5f, 0.5f, 1.0f },{ 0.0f, 1.0f } }, // Top-left

                                                                                   // Bottom face
            { { -1.0f, -1.0f, -1.0f },{ 0.5f, 0.5f, 1.0f, 1.0f },{ 0.0f, 0.0f } }, // Bottom-left
            { { 1.0f, -1.0f, -1.0f },{ 0.5f, 1.0f, 0.5f, 1.0f },{ 1.0f, 0.0f } }, // Bottom-right
            { { 1.0f, -1.0f,  1.0f },{ 1.0f, 0.5f, 0.5f, 1.0f },{ 1.0f, 1.0f } }, // Top-right
            { { -1.0f, -1.0f,  1.0f },{ 1.0f, 1.0f, 0.5f, 1.0f },{ 0.0f, 1.0f } }, // Top-left
        };

        // Define the indices for the cube (two triangles per face)
        std::vector<unsigned int> indices = {
            // Front face
            0, 1, 2, 0, 2, 3,
            // Back face
            4, 5, 6, 4, 6, 7,
            // Left face
            8, 9, 10, 8, 10, 11,
            // Right face
            12, 13, 14, 12, 14, 15,
            // Top face
            16, 17, 18, 16, 18, 19,
            // Bottom face
            20, 21, 22, 20, 22, 23
        };

        Mesh mesh{ vertices, indices };
        m_cube = new MeshObject(mesh);
    }

    /*!
     * \brief Factory destructor.
     */
    CubeDemoFactory::~CubeDemoFactory()
    {
    }

    /*!
     * Gets the name of the Cube Demo
     * 
     * \return Name of the Cube Demo
     */
    std::string CubeDemoFactory::GetName() const
    {
        return "Cube Demo";
    }

    /*!
     * \brief Creates the CubeDemo.
     * 
     * \return A new CubeDemo
     */
    ImDemoInterface * CubeDemoFactory::CreateDemo()
    {
        return new CubeDemo();
    }
}