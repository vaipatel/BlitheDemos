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
        ImVec4 m_clearColor; //!< The main clear color
        float m_aspect;      //!< The main viewport aspect ratio

        std::unordered_set<enPressedKey> m_pressedKeys; //!< The pressed keys, per glfw

        bool m_mouseMoved;
        float m_xPos;
        float m_yPos;
        float m_xOffset;
        float m_yOffset;
    };
}

#endif //UIDATA_H
