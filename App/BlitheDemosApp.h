#ifndef DEMOSTUDIOAPP_H
#define DEMOSTUDIOAPP_H

#include "DemoInterface.h"
#include <list>

struct ImGuiIO;
struct ImVec2;
struct ImVec4;

namespace blithe
{
    class RenderTarget;
    class UIData;

    ///
    /// \brief Hosts the demos.
    ///
    class BlitheDemosApp
    {
    public:
        BlitheDemosApp();
        ~BlitheDemosApp();

        void AddDemoFactory(DemoFactory* _demoFactory);

        void DrawUI(UIData& _uiData);
        void RenderSelectedDemo(const UIData& _uiData);

    private:
        void SelectDemo(DemoFactory* _demoFactory);
        void DrawUIForSelectedDemo(UIData& _uiData);
        void ResizeDefaultViewPortTargetIfNeeded(const ImVec2& _viewportSize);
        void StartDockSpace();
        void EndDockSpace();
        void DoExistingDemoStuff(ImVec4& _clearColor);

        std::list<DemoFactory*> m_demoFactories;          //!< List of demo factories
        DemoFactory* m_selectedDemoFactory = nullptr;     //!< Currently selected demo factory
        DemoInterface* m_currentDemo = nullptr;           //!< Demo created by selected demo factory
        RenderTarget* m_standardViewPortTarget = nullptr; //!< Standard Viewport Render Target

        // ImGui demo flags
        bool m_showDemoWindow = true;
        bool m_showAnotherWindow = false;
    };
}

#endif // DEMOSTUDIOAPP_H
