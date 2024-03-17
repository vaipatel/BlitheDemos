#ifndef IMENGINEAPP_H
#define IMENGINEAPP_H

#include "ImDemoInterface.h"
#include <list>

struct ImGuiIO;
struct ImVec4;

namespace IME
{
    class Texture;
    class UIData;

    class ImEngineApp
    {
    public:
        ImEngineApp();
        ~ImEngineApp();

        void AddDemoFactory(ImDemoFactory* _demoFactory);

        void DrawDemoSelectorUI();
        void DrawUIForSelectedDemo();
        void RenderSelectedDemo(const UIData& _uiData);

    private:
        void DoExistingDemoStuff(ImVec4& _clearColor);

        bool m_showDemoWindow = true;
        bool m_showAnotherWindow = false;

        Texture* m_texture = nullptr;

        std::list<ImDemoFactory*> m_demoFactories;
        ImDemoFactory* m_selectedDemoFactory = nullptr;
        ImDemoInterface* m_currentDemo = nullptr;
    };
}

#endif // IMENGINEAPP_H