#ifndef UIDATA_H
#define UIDATA_H

#include "imgui.h"
#include <unordered_set>

namespace blithe
{
    enum class enPressedKey : int
    {
        KEY_W,
        KEY_A,
        KEY_S,
        KEY_D,
        KEY_ESCAPE
    };

    /*!
     * \brief UI data passed from main to demos
     */
    struct UIData
    {
        bool m_guiCaptured = false;
        bool m_mouseMoved = false;
        bool m_leftDragged = false;
        bool m_scrolled = false;
        float m_xPos = FLT_MIN;
        float m_yPos = FLT_MIN;
        float m_xOffset = FLT_MIN;
        float m_yOffset = FLT_MIN;
        float m_scrollX = FLT_MIN;
        float m_scrollY = FLT_MIN;
        int m_viewportWidth = 0;
        int m_viewPortHeight = 0;
        float m_aspect = FLT_MIN;   //!< The main viewport aspect ratio
        ImVec4 m_clearColor;        //!< The main clear color
        std::unordered_set<enPressedKey> m_pressedKeys; //!< The pressed keys, per glfw
    };
}

#endif //UIDATA_H
