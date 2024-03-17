#include "ImEngineApp.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <algorithm>
#include <stdio.h>

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "ImPath.h"
#include "Texture.h"
#include "UIData.h"

namespace IME
{
    ImEngineApp::ImEngineApp()
    {
        std::string exePath = GetExecutablePath();
        m_texture = new Texture(exePath + "/Assets/vintage_convertible.jpg", TextureData::FilterParam::LINEAR);
        
    }

    ImEngineApp::~ImEngineApp()
    {
        delete m_texture;

        for (ImDemoFactory* fac : m_demoFactories)
        {
            delete fac;
        }
        m_demoFactories.clear();

        delete m_currentDemo;
    }

    void ImEngineApp::AddDemoFactory(ImDemoFactory* _demoFactory)
    {
        ASSERT(_demoFactory, "Demo factory cannot be null");

        std::string name = _demoFactory->GetName();
        bool noDupe = std::find_if(std::begin(m_demoFactories), std::end(m_demoFactories), [name](ImDemoFactory* _fac) { return name == _fac->GetName(); }) == std::end(m_demoFactories);
        ASSERT(noDupe, "Demo factory with name " << name << " has already been added");

        m_demoFactories.push_back(_demoFactory);
    }

    void ImEngineApp::DrawDemoSelectorUI()
    {
        ImGui::Begin("Demo Selector");

        for (ImDemoFactory* demo : m_demoFactories)
        {
            // Use the demo's name as the selectable label
            if (ImGui::Selectable(demo->GetName().c_str(), m_selectedDemoFactory == demo))
            {
                delete m_currentDemo;

                m_selectedDemoFactory = demo;
                m_currentDemo = m_selectedDemoFactory->CreateDemo(); // Set the current demo
                m_currentDemo->OnInit();
            }
        }

        ImGui::End();
    }

    void ImEngineApp::DrawUIForSelectedDemo()
    {
        if (m_currentDemo)
        {
            m_currentDemo->OnImGuiRender();
        }
    }

    void ImEngineApp::RenderSelectedDemo(const UIData& _uiData)
    {
        if (m_currentDemo)
        {
            m_currentDemo->OnRender(_uiData);
        }
    }

    void ImEngineApp::DoExistingDemoStuff(ImVec4& _clearColor)
    {
        ImGuiIO& io = ImGui::GetIO();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (m_showDemoWindow)
            ImGui::ShowDemoWindow(&m_showDemoWindow);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &m_showDemoWindow);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &m_showAnotherWindow);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", reinterpret_cast<float*>(&_clearColor)); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", static_cast<double>(1000.0f / io.Framerate), static_cast<double>(io.Framerate));
            ImGui::End();
        }

        // 3. Show another simple window.
        if (m_showAnotherWindow)
        {
            ImGui::Begin("Another Window", &m_showAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                m_showAnotherWindow = false;
            ImGui::End();
        }
    }
}