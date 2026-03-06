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

Frustum Renderer::RenderMainView(const Camera& camera, const Shader& mainShader, const Shader& lightShader,
                              const Scene& scene, LightingManager& lightingManager, bool useBatching, bool enableCulling)
{
    // Setup projection and view matrices
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 
                                           (float)m_Width / (float)m_Height, 
                                           0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();

    // Create frustum for culling
    Frustum frustum;
    frustum.Update(projection * view);

    // Draw light source
    lightShader.use();
    lightShader.setMat4("projection", projection);
    lightShader.setMat4("view", view);
    scene.RenderLightSource(lightShader);

    // Setup lighting and draw scene
    lightingManager.SetupLighting(mainShader, camera);
    mainShader.setMat4("projection", projection);
    mainShader.setMat4("view", view);

    if (useBatching)
    {
        // Render with batching and optional culling
        const_cast<Scene&>(scene).RenderBatched(mainShader, frustum, enableCulling);
    }
    else
    {
        // Render without batching (legacy mode)
        scene.Render(mainShader);
    }

    return frustum;
}

void Renderer::RenderMiniMap(const Shader& mainShader, const Shader& lightShader, const Scene& scene, 
                            const Frustum& mainCameraFrustum, bool showCullingVisualization, 
                            bool enableCulling, bool hideInvisible)
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

    if (showCullingVisualization && enableCulling)
    {
        // Render with culling visualization only if culling is enabled
        const_cast<Scene&>(scene).RenderWithCullingVisualization(mainShader, mainCameraFrustum, hideInvisible);
    }
    else
    {
        // Render normally without culling (show all objects)
        scene.Render(mainShader);
    }

    // Reset viewport
    glViewport(0, 0, m_Width, m_Height);
}

void Renderer::RenderUI(LightingManager& lightingManager, bool& showMiniMap, bool& useBatching, 
                       bool& enableCulling, bool& hideCulledInMinimap, const Scene& scene)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Lighting controls window
    ImGui::Begin("Lighting Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
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

    // Directional Light
    bool enableDirLight = lightingManager.IsEnableDirLight();
    if (ImGui::Checkbox("Directional Light", &enableDirLight))
    {
        lightingManager.SetEnableDirLight(enableDirLight);
    }

    if (enableDirLight && ImGui::TreeNode("Directional Light Intensity"))
    {
        auto& dirLight = lightingManager.GetDirectionalLight();
        ImGui::SliderFloat("Ambient##Dir", &dirLight.ambientIntensity, 0.0f, 2.0f);
        ImGui::SliderFloat("Diffuse##Dir", &dirLight.diffuseIntensity, 0.0f, 2.0f);
        ImGui::SliderFloat("Specular##Dir", &dirLight.specularIntensity, 0.0f, 2.0f);
        ImGui::TreePop();
    }

    // Point Light
    bool enablePointLight = lightingManager.IsEnablePointLight();
    if (ImGui::Checkbox("Point Light", &enablePointLight))
    {
        lightingManager.SetEnablePointLight(enablePointLight);
    }

    if (enablePointLight && ImGui::TreeNode("Point Light Intensity"))
    {
        auto& pointLight = lightingManager.GetPointLight();
        ImGui::SliderFloat("Ambient##Point", &pointLight.ambientIntensity, 0.0f, 2.0f);
        ImGui::SliderFloat("Diffuse##Point", &pointLight.diffuseIntensity, 0.0f, 2.0f);
        ImGui::SliderFloat("Specular##Point", &pointLight.specularIntensity, 0.0f, 2.0f);
        ImGui::TreePop();
    }

    // Spot Light
    bool enableSpotLight = lightingManager.IsEnableSpotLight();
    if (ImGui::Checkbox("Spot Light", &enableSpotLight))
    {
        lightingManager.SetEnableSpotLight(enableSpotLight);
    }

    if (enableSpotLight && ImGui::TreeNode("Spot Light Intensity"))
    {
        auto& spotLight = lightingManager.GetSpotLight();
        ImGui::SliderFloat("Ambient##Spot", &spotLight.ambientIntensity, 0.0f, 2.0f);
        ImGui::SliderFloat("Diffuse##Spot", &spotLight.diffuseIntensity, 0.0f, 2.0f);
        ImGui::SliderFloat("Specular##Spot", &spotLight.specularIntensity, 0.0f, 2.0f);
        ImGui::TreePop();
    }

    ImGui::Separator();
    ImGui::Text("Rendering Optimization");
    ImGui::Checkbox("Use Batching", &useBatching);
    if (useBatching)
    {
        ImGui::Checkbox("Enable Frustum Culling", &enableCulling);
    }

    ImGui::Separator();
    ImGui::Text("Rendering Statistics");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Total Objects: %u", scene.GetTotalObjects());
    ImGui::Text("Visible Objects: %u", scene.GetVisibleObjects());
    ImGui::Text("Culled Objects: %u", scene.GetCulledObjects());

    if (useBatching)
    {
        ImGui::Text("Batches: %u", scene.GetBatchCount());
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Number of material groups\n(reduced state changes)");
        }

        ImGui::Text("Actual Draw Calls: %u", scene.GetActualDrawCalls());
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Actual GPU draw calls\n(would be reduced with instancing)");
        }
    }
    else
    {
        ImGui::Text("Draw Calls: %u", scene.GetDrawCalls());
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "(No culling/batching)");
    }

    ImGui::Separator();
    ImGui::Text("View Settings");
    ImGui::Checkbox("Show Mini-Map", &showMiniMap);
    m_ShowMiniMap = showMiniMap;

    if (showMiniMap)
    {
        ImGui::Indent();
        ImGui::Text("Minimap Culling Visualization:");
        if (ImGui::RadioButton("Hide Culled Objects", hideCulledInMinimap))
        {
            hideCulledInMinimap = true;
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Objects disappear when culled\n(realistic culling)");
        }

        if (ImGui::RadioButton("Show as Red/Green", !hideCulledInMinimap))
        {
            hideCulledInMinimap = false;
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Green = Visible\nRed = Culled\n(debug visualization)");
        }
        ImGui::Unindent();
    }

    ImGui::Separator();
    ImGui::Checkbox("Show ImGui Demo", &m_ShowImGuiDemo);

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
