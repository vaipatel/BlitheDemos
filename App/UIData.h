#ifndef UIDATA_H
#define UIDATA_H

#include "imgui.h"
#include <glm/glm.hpp>
#include <unordered_set>
#include <vector>
#include "BlitheAssert.h"
#include "KeyMouseEnums.h"

namespace blithe
{
    ///
    /// \brief Tracks drag state for a single mouse button
    ///
    struct MouseButtonDragInfo
    {
        bool m_dragging = false;    //!< Whether the button is currently being dragged
        glm::vec2 m_dragStart = {}; //!< Starting position of the drag
        glm::vec2 m_lastPos = {};   //!< Last recorded mouse position during drag
        glm::vec2 m_delta = {};     //!< Delta since last frame while dragging
    };

    ///
    /// \brief Tracks scrolling input per frame
    ///
    struct MouseScrollState
    {
        float m_xoffset = 0.0f; //!< Horizontal scroll offset (usually unused)
        float m_yoffset = 0.0f; //!< Vertical scroll offset (positive = up, negative = down)

        ///
        /// \brief Resets scroll offsets to zero
        ///
        void Reset()
        {
            m_xoffset = 0.0f;
            m_yoffset = 0.0f;
        }
    };

    ///
    /// \brief Mouse input state per frame (drag, scroll, movement)
    ///
    struct MouseInputState
    {
        //! Per-button drag tracking (left, middle, right)
        std::array<MouseButtonDragInfo, enMouseButtonVec.size()> m_perBtnDragInfos;

        //! Scroll state for this frame
        MouseScrollState m_scroll;

        //! Current mouse cursor position
        glm::vec2 m_mousePos = {};

        //! Mouse cursor position from the previous frame
        glm::vec2 m_mousePrevPos = {};

        //! Cursor delta since the last frame
        glm::vec2 m_mouseDelta = {};

        //! Whether the mouse moved this frame
        bool m_mouseMoved = false;

        //! Whether the mouse was scrolled this frame
        bool m_scrolled = false;

        ///
        /// \brief Retrieves drag info for a specific mouse button
        /// \param _btn - The mouse button
        /// \return Const reference to the corresponding ButtonDragInfo
        ///
        const MouseButtonDragInfo& GetDragInfo(enMouseButton _btn) const
        {
            return m_perBtnDragInfos[static_cast<int>(_btn)];
        }

        ///
        /// \brief Retrieves drag info for a specific mouse button. Non-const version of GetDragInfo().
        /// \param _btn - The mouse button
        /// \return Non-const reference to the corresponding ButtonDragInfo
        ///
        MouseButtonDragInfo& GetModifiableDragInfo(enMouseButton _btn)
        {
            return m_perBtnDragInfos[static_cast<int>(_btn)];
        }

        ///
        /// \brief Retrieves all mouse buttons that are being dragged this frame.
        /// \return All dragged buttons
        ///
        std::vector<enMouseButton> GetDraggedButtons() const
        {
            std::vector<enMouseButton> draggedBtns;
            for ( enMouseButton btn : enMouseButtonVec )
            {
                if ( GetDragInfo(btn).m_dragging )
                {
                    draggedBtns.push_back(btn);
                }
            }
            return draggedBtns;
        }

        ///
        /// \brief Begins tracking a drag for the given mouse button
        /// \param _btn - The button being pressed
        /// \param _pos - The mouse position at the time of press
        ///
        void BeginDrag(enMouseButton _btn, const glm::vec2& _pos)
        {
            MouseButtonDragInfo& info = m_perBtnDragInfos[static_cast<int>(_btn)];
            info.m_dragging = true;
            info.m_dragStart = _pos;
            info.m_lastPos = _pos;
            info.m_delta = glm::vec2(0.0f);
        }

        ///
        /// \brief Ends tracking a drag for the given mouse button
        /// \param _btn - The button being released
        ///
        void EndDrag(enMouseButton _btn)
        {
            MouseButtonDragInfo& info = m_perBtnDragInfos[static_cast<int>(_btn)];
            info.m_dragging = false;
            info.m_delta = glm::vec2(0.0f);
        }

        ///
        /// \brief Updates state with the new mouse position
        /// \param _pos - Current cursor position
        ///
        void UpdateCursorPosition(const glm::vec2& _pos)
        {
            m_mouseDelta.x = _pos.x - m_mousePos.x;
            m_mouseDelta.y = m_mousePos.y - _pos.y; // reversed since y-coordinates go bottom to top
            m_mousePrevPos = m_mousePos;
            m_mousePos = _pos;
            m_mouseMoved = true;

            const std::vector<enMouseButton>& draggedBtns = GetDraggedButtons();
            for ( enMouseButton draggedBtn : draggedBtns )
            {
                MouseButtonDragInfo& info = GetModifiableDragInfo(draggedBtn);
                ASSERT(info.m_dragging, "GetDraggedButtons() reporting incorrect buttons");
                info.m_delta = _pos - info.m_lastPos;
                info.m_lastPos = _pos;
            }
        }

        ///
        /// \brief Updates scroll offsets for this frame
        /// \param _xoffset - Horizontal scroll offset (meaningless as it maybe)
        /// \param _yoffset - Vertical scroll offset
        ///
        void UpdateScroll(double _xoffset, double _yoffset)
        {
            m_scroll.m_xoffset = static_cast<float>(_xoffset);
            m_scroll.m_yoffset = static_cast<float>(_yoffset);
            m_scrolled = true;
        }

        ///
        /// \brief Resets per-frame deltas (drag deltas, scroll, mouse movement)
        ///
        void ResetPerFrame()
        {
            for (MouseButtonDragInfo& btn : m_perBtnDragInfos)
            {
                btn.m_delta = glm::vec2(0.0f);
            }
            m_scroll.Reset();
            m_mouseDelta = glm::vec2(0.0f);
            m_mouseMoved = false;
        }
    };


    /*!
     * \brief UI data passed from main to demos
     */
    struct UIData
    {
        bool m_guiCaptured = false;
        float m_viewPortTopLeftX = 0.0f;
        float m_viewPortTopLeftY = 0.0f;
        int m_viewportWidth = 0;
        int m_viewPortHeight = 0;
        float m_aspect = FLT_MIN;   //!< The main viewport aspect ratio
        ImVec4 m_clearColor;        //!< The main clear color
        std::unordered_set<enPressedKey> m_pressedKeys; //!< The pressed keys, per glfw
        MouseInputState m_mouseInput; //!< Mouse input data
    };
}

#endif //UIDATA_H
