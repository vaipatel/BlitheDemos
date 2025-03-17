#include "BlitheDemosApp.h"
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
#include <tracy/Tracy.hpp>

#include "BlitheAssert.h"
#include "BlitheShared.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "UIData.h"

namespace blithe
{
    ///
    /// \brief Default Constructor.
    ///
    BlitheDemosApp::BlitheDemosApp()
    {
    }

    ///
    /// \brief Destructor. Deletes demo factories, current demo and standard viewport render target.
    ///
    BlitheDemosApp::~BlitheDemosApp()
    {
        for (DemoFactory* fac : m_demoFactories)
        {
            delete fac;
        }
        m_demoFactories.clear();

        delete m_currentDemo;
        delete m_standardViewPortTarget;
    }

    ///
    /// \brief Adds the supplied _demoFactory to list of demo factories. The demo name then shows
    ///        up in the demo selector UI.
    ///
    /// \param _demoFactory - Demo factory for creating a particular demo.
    ///
    void BlitheDemosApp::AddDemoFactory(DemoFactory* _demoFactory)
    {
        ASSERT(_demoFactory, "Demo factory cannot be null");

        std::string name = _demoFactory->GetName();
        bool noDupe = std::find_if(std::begin(m_demoFactories), std::end(m_demoFactories), [name](DemoFactory* _fac) { return name == _fac->GetName(); }) == std::end(m_demoFactories);
        ASSERT(noDupe, "Demo factory with name " << name << " has already been added");

        m_demoFactories.push_back(_demoFactory);
    }

    ///
    /// \brief Draws all the UI elements for the app and any custom elements provided by the demo.
    ///
    /// If the demo had requested to use the standard viewport, this is also where the demo's
    /// rendered contents, available in the standard viewport render target's texture are submitted
    /// for drawing on the standard viewport using an ImGui::Image().
    ///
    void BlitheDemosApp::DrawUI(UIData& _uiData)
    {
        ZoneScoped;

        // The entire app UI is a DockSpace. So begin the DockSpace.
        StartDockSpace();

        // ----
        // Draw the pane to select demos.
        ImGui::Begin("Demo Selector");

        for (DemoFactory* demoFac : m_demoFactories)
        {
            // Use the demo's name as the selectable label
            if (ImGui::Selectable(demoFac->GetName().c_str(), m_selectedDemoFactory == demoFac))
            {
                SelectDemo(demoFac);
            }
        }

        ImGui::End();

        // Draw to the standard viewport and also draw any custom UI provided by the demo.
        DrawUIForSelectedDemo(_uiData);
        // ----

        EndDockSpace();
    }

    ///
    /// \brief Called by main to give any selected demo the chance to make OpenGL calls and render
    ///        things.
    ///
    /// \param _uiData - Data about the main viewport that might be useful for rendering
    ///
    void BlitheDemosApp::RenderSelectedDemo(const UIData& _uiData)
    {
        ZoneScoped;

        UIData uiData = _uiData;

        if (m_currentDemo)
        {
            // If the demo had requested to use the standard viewport, we first bind the standard
            // viewport render target's framebuffer. This way all the demo's rendering will end up
            // in the standard viewport render target texture. This texture can then submitted to
            // an ImGui::Image() to display the rendered contents in the standard viewport.
            if (m_currentDemo->UsesStandardViewPort())
            {
                if (m_standardViewPortTarget)
                {
                    m_standardViewPortTarget->Bind();

                    int textureWidth = m_standardViewPortTarget->GetTargetTexture()->GetWidth();
                    int textureHeight = m_standardViewPortTarget->GetTargetTexture()->GetHeight();
                    glViewport(0, 0, textureWidth, textureHeight);

                    float aspect = static_cast<float>(textureWidth) / static_cast<float>(textureHeight);
                    glm::vec4 clearColor = m_standardViewPortTarget->GetClearColor();
                    ImVec4 clearCol(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
                    uiData.m_clearColor = clearCol;
                    uiData.m_aspect = aspect;
                }
            }

            // Give demo a chance to do its rendering.
            double currentTimeS = glfwGetTime(); // Get elapsed time since GLFW started
            double deltaTimeS = currentTimeS - m_lastFrameTimeS;
            m_lastFrameTimeS = currentTimeS;
            m_currentDemo->OnRender(deltaTimeS, uiData);

            // Finally, if we had bound the standard viewport framebuffer at the demo's behest,
            // unbind it now.
            if (m_currentDemo->UsesStandardViewPort())
            {
                if (m_standardViewPortTarget)
                {
                    m_standardViewPortTarget->UnBind();
                }
            }
        }
    }

    ///
    /// \brief Creates and initializes the demo created by _demoFactory. Deletes any existing demo
    ///        first.
    ///
    /// \param _demoFactory - Factory for creating the demo
    ///
    void BlitheDemosApp::SelectDemo(DemoFactory* _demoFactory)
    {
        DeleteAndNull(m_currentDemo);

        m_selectedDemoFactory = _demoFactory;
        m_currentDemo = m_selectedDemoFactory->CreateDemo(); // Set the current demo
        m_currentDemo->OnInit();

        m_lastFrameTimeS = glfwGetTime();
    }

    ///
    /// \brief Draws the current demo's rendered contents to the standard viewport, if the demo had
    ///        requested that facility, and then gives the demo a chance to draw any custom UI it
    ///        needs.
    ///
    void BlitheDemosApp::DrawUIForSelectedDemo(UIData& _uiData)
    {
        if (m_currentDemo)
        {
            // If a demo had requested that its rendered content be drawn to the standard viewport,
            // this class would have made sure to bind the standard viewport render target's
            // framebuffer before calling the demo's render code. This way all the demo rendering
            // would have gone to the standard viewport's framebuffer. Having done that, all this
            // function needs to do is the standard viewport render target's texture to an
            // ImGui::Image().
            if (m_currentDemo->UsesStandardViewPort())
            {
                ImGui::Begin("Standard Viewport");

                // Resize the standard viewport render target if needed
                ImVec2 viewportSize = ImGui::GetContentRegionAvail();
                ResizeDefaultViewPortTargetIfNeeded(viewportSize);

                if (m_standardViewPortTarget)
                {
                    // Convert GLuint to ImTextureID (a.k.a. void*) for ImGui
                    ImTextureID texID = static_cast<ImTextureID>(m_standardViewPortTarget->GetTargetTexture()->GetHandle());

                    // Display the texture in the ImGui window
                    int textureWidth = m_standardViewPortTarget->GetTargetTexture()->GetWidth();
                    int textureHeight = m_standardViewPortTarget->GetTargetTexture()->GetHeight();
                    ImVec2 viewportSize(static_cast<float>(textureWidth), static_cast<float>(textureHeight));
                    // We're passing an OpenGL texture that goes bottom left (0,0) to top right (1,1)
                    // into ImGui that wants top left (0,0) to bottom right (1,1). So we tell ImGui
                    // to go from our top left (0,1) to our bottom right (1,0).
                    ImVec2 uv0(0, 1);
                    ImVec2 uv1(1, 0);
                    ImGui::Image(texID, viewportSize, uv0, uv1);

                    // TODO: Read more of https://github.com/ocornut/imgui/issues/4234
                    // and possibly https://github.com/ocornut/imgui/issues/5708
                    bool isHovered = ImGui::IsItemHovered();
                    bool isFocused = ImGui::IsItemFocused();
                    _uiData.m_guiCaptured = !(isHovered || isFocused);
                    //std::cout << isHovered << " " << isFocused << std::endl;
                }

                ImGui::End();
            }

            // Draw the demo's custom UI.
            m_currentDemo->OnDrawUI();
        }
    }

    ///
    /// \brief Resizes the standard viewport render target texture if its dimensions do not match
    ///        the given _viewPortSize. (Creates the render target to the _viewPortSize if it does
    ///        not exist yet).
    ///
    /// \param _viewportSize - Viewport Size to use for resizing the render target texture
    ///
    void BlitheDemosApp::ResizeDefaultViewPortTargetIfNeeded(const ImVec2& _viewportSize)
    {
        if (m_standardViewPortTarget)
        {
            int textureWidth = m_standardViewPortTarget->GetTargetTexture()->GetWidth();
            int textureHeight = m_standardViewPortTarget->GetTargetTexture()->GetHeight();

            // If the viewport is resized, update the texture and framebuffer
            if (static_cast<int>(_viewportSize.x) != textureWidth || static_cast<int>(_viewportSize.y) != textureHeight)
            {
                // I'm just deleting the RenderTarget for now. Gross.
                DeleteAndNull(m_standardViewPortTarget);
            }
        }

        // If we deleted the target above, or didn't have one to begin with, create it
        if (!m_standardViewPortTarget)
        {
            int textureWidth = static_cast<int>(_viewportSize.x);
            int textureHeight = static_cast<int>(_viewportSize.y);

            // Call function to resize framebuffer and texture, but only if width and height are non zero
            if (textureWidth > 0 && textureHeight > 0)
            {
                m_standardViewPortTarget = new RenderTarget(textureWidth, textureHeight, GL_RGBA, GL_UNSIGNED_BYTE, TextureData::FilterParam::LINEAR);
                m_standardViewPortTarget->SetDepthTestEnabled(true);
                m_standardViewPortTarget->SetClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                m_standardViewPortTarget->SetClearColor({ 0.15, 0.15, 0.15, 1.0 });
            }
        }
    }

    ///
    /// \brief Setup a Dock Space for the main viewport.
    ///
    ///        I couldn't get this going by just following the advice in the example to
    ///        just ImGui::DockSpaceOverViewport(ImGui::GetMainViewPort()).
    ///        So I stole this from what the demo actually does. More specifically, I
    ///        stole from TheCherno going over the ImGui DockSpace example in 
    ///        "BEST WAY to make Desktop Applications in C++" https://www.youtube.com/watch?v=vWXrFetSH8w
    ///
    void BlitheDemosApp::StartDockSpace()
    {
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", nullptr, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                ImGui::MenuItem("Padding", NULL, &opt_padding);
                ImGui::Separator();

                if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode; }
                if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit; }
                if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking; }
                if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
                if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
                if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
                ImGui::Separator();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

    ///
    /// \brief A bit awkward, but ends what StartDockSpace() started.
    ///        Crucially, all the other UI stuff should go between StartDockSpace() and EndDockSpace().
    ///
    void BlitheDemosApp::EndDockSpace()
    {
        ImGui::End();
    }

    void BlitheDemosApp::DoExistingDemoStuff(ImVec4& _clearColor)
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
