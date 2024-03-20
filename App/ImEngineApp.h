#ifndef IMENGINEAPP_H
#define IMENGINEAPP_H

#include "ImDemoInterface.h"
#include <list>

struct ImGuiIO;
struct ImVec2;
struct ImVec4;

namespace IME
{
	class RenderTarget;
    class UIData;

    class ImEngineApp
    {
    public:
        ImEngineApp();
        ~ImEngineApp();

        void AddDemoFactory(ImDemoFactory* _demoFactory);

        void DrawUI();
        void RenderSelectedDemo(const UIData& _uiData);

    private:
		void SelectDemo(ImDemoFactory* _demoFactory);
		void DrawUIForSelectedDemo();
		void ResizeDefaultViewPortTargetIfNeeded(const ImVec2& _viewportSize);
		void StartDockSpace();
		void EndDockSpace();
        void DoExistingDemoStuff(ImVec4& _clearColor);

        bool m_showDemoWindow = true;
        bool m_showAnotherWindow = false;

        std::list<ImDemoFactory*> m_demoFactories;
        ImDemoFactory* m_selectedDemoFactory = nullptr;
        ImDemoInterface* m_currentDemo = nullptr;
		RenderTarget* m_target = nullptr;
    };
}

#endif // IMENGINEAPP_H