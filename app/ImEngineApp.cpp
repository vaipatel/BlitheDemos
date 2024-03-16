#include "ImEngineApp.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "ImPath.h"
#include "ShaderProgram.h"
//#include "Texture.h"
#include "TrisObject.h"

ImEngineApp::ImEngineApp()
{
	std::string exePath = IME::GetExecutablePath();
	m_shader = new ShaderProgram(exePath + "/Shaders/Triangle.vert", exePath + "/Shaders/Triangle.frag");
	//m_texture = new Texture("P1030293.jpg", TextureData::FilterParam::LINEAR, 0);
	SetupTriangle();
}

ImEngineApp::~ImEngineApp()
{
	delete m_shader;
	delete m_texture;
	delete m_tri;
}

void ImEngineApp::Render(const ImVec4& _clearColor)
{
	m_shader->Bind();
	//
	////    QMatrix4x4 matrix;
	////    matrix.perspective(60.0f, m_viewAspect, 0.1f, 100.0f);
	////    matrix.translate(0, 0, -4);
	////    float angle = static_cast<float>(100.0 * m_frame / screen()->refreshRate());
	////    matrix.rotate(angle, 0, 1, 0);
	////    m_triangleProgram->SetUniform("matrix", matrix);
	////    m_triangleProgram->SetUniform("angle", angle);
	//
	m_tri->Draw();
	
	m_shader->Unbind();
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

void ImEngineApp::SetupTriangle()
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