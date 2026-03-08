#pragma once
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Shader.h"
#include "Scene.h"
#include "LightingManager.h"
#include "InputManager.h"
#include "Renderer.h"
#include "Frustum.h"

class Application
{
public:
    Application(int width = 1280, int height = 720);
    ~Application();

    bool Initialize();
    void Run();

private:
    void Update(float deltaTime);
    void Render();

    // Window
    GLFWwindow* m_Window;
    int m_Width;
    int m_Height;

    // Core systems
    std::unique_ptr<Camera> m_Camera;
    std::unique_ptr<Camera> m_MiniMapCamera;
    std::unique_ptr<Scene> m_Scene;
    std::unique_ptr<LightingManager> m_LightingManager;
    std::unique_ptr<InputManager> m_InputManager;
    std::unique_ptr<Renderer> m_Renderer;

    // Shaders
    std::unique_ptr<Shader> m_MainShader;
    std::unique_ptr<Shader> m_LightingShader;
    std::unique_ptr<Shader> m_ShadowDepthShader;

    // Timing
    float m_DeltaTime;
    float m_LastFrame;

    // UI State
    bool m_ShowMiniMap;
    bool m_UseBatching;
    bool m_EnableCulling;
    bool m_HideCulledInMinimap;
    bool m_EnableNormalMapping;

    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
};
