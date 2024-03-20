#ifndef UIDATA_H
#define UIDATA_H

#include "imgui.h"

namespace BLE
{
    /*!
     * \brief UI data passed from main to demos
     */
    struct UIData
    {
        ImVec4 m_clearColor; //!< The main clear color
        float m_aspect;      //!< The main viewport aspect ratio
    };
}

#endif //UIDATA_H