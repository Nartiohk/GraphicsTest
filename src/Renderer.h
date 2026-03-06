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
#include "Frustum.h"

class Renderer
{
public:
    Renderer(GLFWwindow* window, int width, int height);
    ~Renderer();

    void BeginFrame();
    Frustum RenderMainView(const Camera& camera, const Shader& mainShader, const Shader& lightShader, 
                       const Scene& scene, LightingManager& lightingManager, bool useBatching = true, bool enableCulling = true);
    void RenderMiniMap(const Shader& mainShader, const Shader& lightShader, const Scene& scene, 
                      const Frustum& mainCameraFrustum, bool showCullingVisualization = true, 
                      bool enableCulling = true, bool hideInvisible = true);
    void RenderUI(LightingManager& lightingManager, bool& showMiniMap, bool& useBatching, 
                 bool& enableCulling, bool& hideCulledInMinimap, const Scene& scene);
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
