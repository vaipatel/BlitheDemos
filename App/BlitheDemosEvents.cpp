#include "BlitheDemosEvents.h"

blithe::UIData BlitheDemosEvents::s_uiData;

// (stolen/adapted from learnopengl)
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void BlitheDemosEvents::ProcessInput(GLFWwindow *_window)
{
    std::unordered_set<blithe::enPressedKey>& pressedKeys = s_uiData.m_pressedKeys;
    pressedKeys.clear();

    if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        pressedKeys.insert(blithe::enPressedKey::KEY_ESCAPE);
        glfwSetWindowShouldClose(_window, true);
    }

    if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
    {
        pressedKeys.insert(blithe::enPressedKey::KEY_W);
    }
    if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
    {
        pressedKeys.insert(blithe::enPressedKey::KEY_S);
    }
    if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
    {
        pressedKeys.insert(blithe::enPressedKey::KEY_A);
    }
    if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
    {
        pressedKeys.insert(blithe::enPressedKey::KEY_D);
    }
}

// (stolen/adapted from learnopengl)
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void BlitheDemosEvents::CursorPosCallback(GLFWwindow* _window, double _xpos, double _ypos)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(_xpos, _ypos);

    blithe::MouseInputState& mouseInput = s_uiData.m_mouseInput;
    mouseInput.UpdateCursorPosition({_xpos, _ypos});

    // Populate uiData
    s_uiData.m_guiCaptured = io.WantCaptureMouse;

    // static uint64_t s_mousePrintIdx = 0;
    // std::cout << "Mouse event: " << " print idx = " << s_mousePrintIdx++
    //           << ", xoffset = " << mouseInput.m_mouseDelta.x
    //           << ", yoffset = " << mouseInput.m_mouseDelta.y
    //           << ", leftDragged = " << mouseInput.Get(blithe::enMouseButton::Left).m_dragging
    //           << std::endl;
}

void BlitheDemosEvents::MouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods)
{
    double xpos, ypos;
    glfwGetCursorPos(_window, &xpos, &ypos);
    glm::vec2 pos(xpos, ypos);

    bool btnKnown = _button == GLFW_MOUSE_BUTTON_LEFT
                    || _button == GLFW_MOUSE_BUTTON_RIGHT
                    || _button == GLFW_MOUSE_BUTTON_MIDDLE;
    if ( btnKnown )
    {
        blithe::enMouseButton btn = static_cast<blithe::enMouseButton>(_button);
        if (_action == GLFW_PRESS)
        {
            s_uiData.m_mouseInput.BeginDrag(btn, pos);
        }
        else if (_action == GLFW_RELEASE)
        {
            s_uiData.m_mouseInput.EndDrag(btn);
        }
    }
}

void BlitheDemosEvents::ScrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseWheelEvent(_xoffset, _yoffset);

    s_uiData.m_mouseInput.UpdateScroll(_xoffset, _yoffset);
}

void BlitheDemosEvents::ResetUIDataPerFrame()
{
    s_uiData.m_guiCaptured = false;
    s_uiData.m_viewportWidth = 0;
    s_uiData.m_viewPortHeight = 0;
    s_uiData.m_aspect = FLT_MIN;
    s_uiData.m_pressedKeys.clear();
    s_uiData.m_mouseInput.ResetPerFrame();
}
