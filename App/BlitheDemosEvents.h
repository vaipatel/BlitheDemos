#ifndef BLITHEDEMOSEVENTS_H
#define BLITHEDEMOSEVENTS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <glm/glm.hpp>
#include <map>
#include "UIData.h"

class BlitheDemosEvents
{
public:
    static void ProcessInput(GLFWwindow* _window);
    static void CursorPosCallback(GLFWwindow* _window, double _xpos, double _ypos);
    static void MouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods);
    static void ScrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset);
    static void ResetUIDataPerFrame();

    static blithe::UIData s_uiData;

private:
    static std::map<int, blithe::enMouseButton> s_glfwToBlitheMouseBtns;
};

#endif // BLITHEDEMOSEVENTS_H
