#include "Renderer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Renderer::Renderer(GLFWwindow* window, int width, int height)
    : m_Window(window)
    , m_Width(width)
    , m_Height(height)
    , m_ShowMiniMap(true)
    , m_ShowImGuiDemo(false)
    , m_MiniMapWidth(320)
    , m_MiniMapHeight(240)
{
    m_MiniMapX = m_Width - m_MiniMapWidth - 10;
    m_MiniMapY = 10;

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init();
}

Renderer::~Renderer()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Renderer::BeginFrame()
{
    glViewport(0, 0, m_Width, m_Height);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::RenderMainView(const Camera& camera, const Shader& mainShader, const Shader& lightShader,
                              const Scene& scene, LightingManager& lightingManager)
{
    // Setup projection and view matrices
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 
                                           (float)m_Width / (float)m_Height, 
                                           0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();

    // Draw light source
    lightShader.use();
    lightShader.setMat4("projection", projection);
    lightShader.setMat4("view", view);
    scene.RenderLightSource(lightShader);

    // Setup lighting and draw scene
    lightingManager.SetupLighting(mainShader, camera);
    mainShader.setMat4("projection", projection);
    mainShader.setMat4("view", view);
    scene.Render(mainShader);
}

void Renderer::RenderMiniMap(const Shader& mainShader, const Shader& lightShader, const Scene& scene)
{
    if (!m_ShowMiniMap)
        return;

    // Clear mini-map area
    glEnable(GL_SCISSOR_TEST);
    glScissor(m_MiniMapX, m_MiniMapY, m_MiniMapWidth, m_MiniMapHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    // Set viewport for mini-map
    glViewport(m_MiniMapX, m_MiniMapY, m_MiniMapWidth, m_MiniMapHeight);

    // Setup mini-map projection and view (looking at scene from above/angle)
    glm::mat4 miniMapProjection = glm::perspective(
        glm::radians(45.0f),
        (float)m_MiniMapWidth / (float)m_MiniMapHeight,
        0.1f,
        100.0f
    );
    // Use direct lookAt to point camera at scene center
    glm::mat4 miniMapView = glm::lookAt(
        glm::vec3(0.0f, 15.0f, 15.0f),  // Camera position
        glm::vec3(0.0f, 0.0f, 0.0f),    // Look at center
        glm::vec3(0.0f, 1.0f, 0.0f)     // Up vector
    );

    // Draw light source in mini-map
    lightShader.use();
    lightShader.setMat4("projection", miniMapProjection);
    lightShader.setMat4("view", miniMapView);
    scene.RenderLightSource(lightShader);

    // Draw scene in mini-map
    mainShader.use();
    mainShader.setMat4("projection", miniMapProjection);
    mainShader.setMat4("view", miniMapView);
    scene.Render(mainShader);

    // Reset viewport
    glViewport(0, 0, m_Width, m_Height);
}

void Renderer::RenderUI(LightingManager& lightingManager, bool& showMiniMap)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Lighting controls window
    ImGui::Begin("Lighting Controls");
    ImGui::Text("Press TAB to toggle cursor");
    ImGui::Separator();
    
    ImGui::Text("Shadow Settings");
    bool enableShadows = lightingManager.IsEnableShadows();
    if (ImGui::Checkbox("Enable Shadows", &enableShadows))
    {
        lightingManager.SetEnableShadows(enableShadows);
    }
    
    ImGui::Separator();
    ImGui::Text("Light Sources");
    
    bool enableDirLight = lightingManager.IsEnableDirLight();
    if (ImGui::Checkbox("Directional Light", &enableDirLight))
    {
        lightingManager.SetEnableDirLight(enableDirLight);
    }
    
    bool enablePointLight = lightingManager.IsEnablePointLight();
    if (ImGui::Checkbox("Point Light", &enablePointLight))
    {
        lightingManager.SetEnablePointLight(enablePointLight);
    }
    
    bool enableSpotLight = lightingManager.IsEnableSpotLight();
    if (ImGui::Checkbox("Spot Light", &enableSpotLight))
    {
        lightingManager.SetEnableSpotLight(enableSpotLight);
    }
    
    ImGui::Separator();
    ImGui::Text("View Settings");
    ImGui::Checkbox("Show Mini-Map", &showMiniMap);
    m_ShowMiniMap = showMiniMap;

    ImGui::Separator();
    ImGui::Checkbox("Show ImGui Demo", &m_ShowImGuiDemo);

    ImGui::Separator();
    ImGui::Text("Application %.1f FPS", ImGui::GetIO().Framerate);
    ImGui::End();

    // Optional ImGui demo window
    if (m_ShowImGuiDemo)
    {
        ImGui::ShowDemoWindow(&m_ShowImGuiDemo);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::EndFrame()
{
    glfwSwapBuffers(m_Window);
}
