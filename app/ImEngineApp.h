#ifndef IMENGINEAPP_H
#define IMENGINEAPP_H

struct ImGuiIO;
struct ImVec4;

class ShaderProgram;
class Texture;
class TrisObject;

class ImEngineApp
{
public:
	ImEngineApp();
	~ImEngineApp();

	void Render(ImVec4& _clearColor);

private:
	void DoExistingDemoStuff(ImVec4& _clearColor);
	void SetupTriangle();

	bool m_showDemoWindow = true;
	bool m_showAnotherWindow = false;

	ShaderProgram* m_shader = nullptr;
	Texture* m_texture = nullptr;
	TrisObject* m_tri = nullptr;
};

#endif // IMENGINEAPP_H