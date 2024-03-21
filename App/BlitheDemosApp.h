#ifndef DEMOSTUDIOAPP_H
#define DEMOSTUDIOAPP_H

#include "DemoInterface.h"
#include <list>

struct ImGuiIO;
struct ImVec2;
struct ImVec4;

namespace BLE
{
    class RenderTarget;
    class UIData;

    /*!
     * \brief Hosts the demos.
     */
    class BlitheDemosApp
    {
    public:
        BlitheDemosApp();
        ~BlitheDemosApp();

        void AddDemoFactory(DemoFactory* _demoFactory);

        void DrawUI();
        void RenderSelectedDemo(const UIData& _uiData);

    private:
        void SelectDemo(DemoFactory* _demoFactory);
        void DrawUIForSelectedDemo();
        void ResizeDefaultViewPortTargetIfNeeded(const ImVec2& _viewportSize);
        void StartDockSpace();
        void EndDockSpace();
        void DoExistingDemoStuff(ImVec4& _clearColor);

        bool m_showDemoWindow = true;
        bool m_showAnotherWindow = false;

        std::list<DemoFactory*> m_demoFactories;
        DemoFactory* m_selectedDemoFactory = nullptr;
        DemoInterface* m_currentDemo = nullptr;
        RenderTarget* m_target = nullptr;
    };
}

#endif // DEMOSTUDIOAPP_H