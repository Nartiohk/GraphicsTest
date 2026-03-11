#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Camera.h"

class InputManager
{
public:
    InputManager(GLFWwindow* window, Camera& camera);
    ~InputManager() = default;

    void ProcessInput(float deltaTime);
    bool IsCursorDisabled() const { return m_CursorDisabled; }

    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void SetInstance(InputManager* instance) { s_Instance = instance; }

private:
    void ProcessMouseMovement(double xpos, double ypos);

    GLFWwindow* m_Window;
    Camera& m_Camera;
    
    float m_LastX;
    float m_LastY;
    bool m_FirstMouse;
    bool m_CursorDisabled;
    bool m_TabPressed;

    static InputManager* s_Instance;
};
