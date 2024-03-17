#include "TriangleDemo.h"
#include "ImPath.h"
#include "ShaderProgram.h"
#include "TrisObject.h"

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
    TriangleDemo::~TriangleDemo()
    {
        delete m_shader;
        delete m_tri;
    }

    /*!
     * \brief Setup for the triangle demo
     */
    void TriangleDemo::OnInit()
    {
        std::string exePath = GetExecutablePath();
        m_shader = new ShaderProgram(exePath + "/Shaders/Triangle.vert", exePath + "/Shaders/Triangle.frag");
        SetupTriangle();

        m_lastFrameTime = glfwGetTime();
    }

    /*!
     * \brief Render the triangle
     */
    void TriangleDemo::OnRender(float _aspect)
    {
        const float pi = glm::pi<float>();

        float currentTime = glfwGetTime(); // Get elapsed time since GLFW started
        float deltaTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;

        m_rotationAngleRad += deltaTime * m_rotationSpeed * 2.0f * pi; // Update based on speed
        m_rotationAngleRad = fmod(m_rotationAngleRad, 2.0f * pi); // Keep progress within a full rotation range

        glm::mat4 matrix = glm::perspective(pi/3.0f, _aspect, 0.1f, 100.0f);
        matrix = glm::translate(matrix, { 0, 0, -2.0f });
        matrix = glm::rotate(matrix, m_rotationAngleRad, glm::vec3(0.0f, 1.0f, 0.0f));

        m_shader->Bind();
        m_shader->SetUniformMat4f("matrix", matrix);

        m_tri->Draw();

        m_shader->Unbind();
    }

    /*!
     * \brief ImGui controls specific to the triangle demo
     */
    void TriangleDemo::OnImGuiRender()
    {
        ImGui::Begin("Triangle Demo Params");

        ImGui::SliderFloat("Rotation Speed", &m_rotationSpeed, 0.0f, 1.0f);

        ImGui::End();
    }

    /*!
     * \brief Helper to setup the triangle
     */
    void TriangleDemo::SetupTriangle()
    {
        std::vector<Tri> triVertices = {
            // positions            // colors                 // texCoords
            {
                { { 0.0f,  0.707f, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f } },
                { { -0.5f, -0.5f,   0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f },{ 0.0f, 0.0f } },
                { { 0.5f, -0.5f,   0.0f },{ 0.0f, 0.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } }
            }
        };
        m_tri = new TrisObject(triVertices);
    }

    /*!
     * \brief Factory destructor.
     */
    TriangleDemoFactory::~TriangleDemoFactory()
    {
    }

    /*!
     * Gets the name of the Triangle Demo
     * 
     * \return Name of the Triangle Demo
     */
    std::string TriangleDemoFactory::GetName() const
    {
        return "Triangle Demo";
    }

    /*!
     * \brief Creates the TriangleDemo.
     * 
     * \return A new TriangleDemo
     */
    ImDemoInterface * TriangleDemoFactory::CreateDemo()
    {
        return new TriangleDemo();
    }
}