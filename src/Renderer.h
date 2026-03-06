#pragma once
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "Camera.h"
#include "Shader.h"
#include "Scene.h"
#include "LightingManager.h"
#include "InputManager.h"

class Renderer
{
public:
    Renderer(GLFWwindow* window, int width, int height);
    ~Renderer();

    void BeginFrame();
    void RenderMainView(const Camera& camera, const Shader& mainShader, const Shader& lightShader, 
                       const Scene& scene, LightingManager& lightingManager);
    void RenderMiniMap(const Shader& mainShader, const Shader& lightShader, const Scene& scene);
    void RenderUI(LightingManager& lightingManager, bool& showMiniMap);
    void EndFrame();

    bool ShouldShowMiniMap() const { return m_ShowMiniMap; }
    void SetShowMiniMap(bool show) { m_ShowMiniMap = show; }

private:
    GLFWwindow* m_Window;
    int m_Width;
    int m_Height;
    bool m_ShowMiniMap;
    bool m_ShowImGuiDemo;

    // Mini-map settings
    int m_MiniMapWidth;
    int m_MiniMapHeight;
    int m_MiniMapX;
    int m_MiniMapY;
};
