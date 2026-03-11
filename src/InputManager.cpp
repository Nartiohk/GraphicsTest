#include "InputManager.h"

InputManager* InputManager::s_Instance = nullptr;

InputManager::InputManager(GLFWwindow* window, Camera& camera)
    : m_Window(window)
    , m_Camera(camera)
    , m_LastX(640.0f)
    , m_LastY(360.0f)
    , m_FirstMouse(true)
    , m_CursorDisabled(true)
    , m_TabPressed(false)
{
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void InputManager::ProcessInput(float deltaTime)
{
    // ESC to close window
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(m_Window, true);
    }

    // TAB to toggle cursor
    if (glfwGetKey(m_Window, GLFW_KEY_TAB) == GLFW_PRESS && !m_TabPressed)
    {
        m_TabPressed = true;
        m_CursorDisabled = !m_CursorDisabled;
        
        if (m_CursorDisabled)
        {
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            m_FirstMouse = true;
        }
        else
        {
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
    
    if (glfwGetKey(m_Window, GLFW_KEY_TAB) == GLFW_RELEASE)
    {
        m_TabPressed = false;
    }

    // Camera movement only when cursor is disabled
    if (!m_CursorDisabled)
        return;

    if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(0, deltaTime);
    if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(1, deltaTime);
    if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(2, deltaTime);
    if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(3, deltaTime);
    if (glfwGetKey(m_Window, GLFW_KEY_Q) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(4, deltaTime);
    if (glfwGetKey(m_Window, GLFW_KEY_E) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(5, deltaTime);
}

void InputManager::ProcessMouseMovement(double xpos, double ypos)
{
    if (!m_CursorDisabled)
        return;

    if (m_FirstMouse)
    {
        m_LastX = (float)xpos;
        m_LastY = (float)ypos;
        m_FirstMouse = false;
    }

    float xoffset = (float)xpos - m_LastX;
    float yoffset = m_LastY - (float)ypos;
    m_LastX = (float)xpos;
    m_LastY = (float)ypos;

    m_Camera.ProcessMouseMovement(xoffset, yoffset);
}

void InputManager::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (s_Instance)
    {
        s_Instance->ProcessMouseMovement(xpos, ypos);
    }
}
