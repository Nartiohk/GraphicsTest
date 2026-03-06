#include "Scene.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Scene::Scene()
    : m_Plane(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(10.0f, 1.0f, 10.0f), glm::vec3(0.0f), glm::vec3(0.3f, 0.3f, 0.3f))
    , m_Sphere(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f), glm::vec3(0.0f), glm::vec3(1.0f, 0.2f, 0.3f))
    , m_LightSource(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.1f), glm::vec3(0.0f), glm::vec3(1.0f, 1.0f, 1.0f))
    , m_TotalObjects(0)
    , m_VisibleObjects(0)
    , m_DrawCalls(0)
    , m_BatchCount(0)
    , m_ActualDrawCalls(0)
{
    // Load textures and create materials
    LoadTextures();
    CreateMaterials();

    // Create multiple cube instances in a circle
    glm::vec3 cubeColor(0.0f, 0.5f, 0.31f);
    for (int i = 0; i < 5; ++i)
    {
        float angle = (float)i / 5.0f * 360.0f;
        float radius = 3.0f;
        float x = cos(glm::radians(angle)) * radius;
        float z = sin(glm::radians(angle)) * radius;
        auto cube = std::make_unique<Cube>(
            glm::vec3(x, 0.0f, z),
            glm::vec3(1.0f),
            glm::vec3(0.0f),
            cubeColor
        );

        // Alternate between brick and container materials
        if (i % 2 == 0 && m_BrickMaterial)
        {
            cube->SetMaterial(m_BrickMaterial);
        }
        else if (m_ContainerMaterial)
        {
            cube->SetMaterial(m_ContainerMaterial);
        }

        m_Cubes.push_back(std::move(cube));
    }

    // Set brick material on plane for better visibility of normal mapping
    if (m_BrickMaterial)
    {
        m_Plane.SetMaterial(m_BrickMaterial);
    }
}

void Scene::LoadTextures()
{
    std::cout << "Loading textures..." << std::endl;

    // This function is now just a placeholder since textures are loaded in CreateMaterials
}

void Scene::CreateMaterials()
{
    std::cout << "Creating materials..." << std::endl;

    // Create brick material with diffuse and normal map
    m_BrickMaterial = std::make_shared<Material>();

    auto brickDiffuse = std::make_shared<Texture>();
    if (brickDiffuse->LoadFromFile("../textures/brickwall.jpg"))
    {
        m_BrickMaterial->SetDiffuseTexture(brickDiffuse);
    }

    auto brickNormal = std::make_shared<Texture>();
    if (brickNormal->LoadFromFile("../textures/brickwall_normal.jpg"))
    {
        m_BrickMaterial->SetNormalTexture(brickNormal);
    }

    // Create a procedural specular map for bricks (they should be less shiny)
    auto brickSpecular = std::make_shared<Texture>();
    brickSpecular->CreateSolidColor(64, 64, 64); // Medium gray = medium specularity
    m_BrickMaterial->SetSpecularTexture(brickSpecular);
    m_BrickMaterial->SetShininess(16.0f); // Less shiny than default

    std::cout << "Brick material created with "
              << (m_BrickMaterial->HasDiffuseTexture() ? "diffuse" : "no diffuse") << ", "
              << (m_BrickMaterial->HasNormalTexture() ? "normal" : "no normal") << ", "
              << (m_BrickMaterial->HasSpecularTexture() ? "specular" : "no specular") << std::endl;

    // Create container material
    m_ContainerMaterial = std::make_shared<Material>();

    auto containerDiffuse = std::make_shared<Texture>();
    if (containerDiffuse->LoadFromFile("../textures/container.jpg"))
    {
        m_ContainerMaterial->SetDiffuseTexture(containerDiffuse);
    }

    // Wood should be fairly shiny
    auto containerSpecular = std::make_shared<Texture>();
    containerSpecular->CreateSolidColor(200, 200, 200); // Bright = very shiny
    m_ContainerMaterial->SetSpecularTexture(containerSpecular);
    m_ContainerMaterial->SetShininess(64.0f); // Shiny wood

    std::cout << "Container material created with "
              << (m_ContainerMaterial->HasDiffuseTexture() ? "diffuse" : "no diffuse") << ", "
              << (m_ContainerMaterial->HasSpecularTexture() ? "specular" : "no specular") << std::endl;
}

void Scene::Render(const Shader& shader) const
{
    // Update statistics for non-batched rendering
    m_TotalObjects = m_Cubes.size() + 2; // cubes + plane + sphere
    m_VisibleObjects = m_TotalObjects; // No culling in regular render
    m_DrawCalls = m_TotalObjects; // No batching = 1 draw call per object
    m_BatchCount = 0; // No batching
    m_ActualDrawCalls = m_TotalObjects; // Same as draw calls without batching

    for (const auto& cube : m_Cubes)
    {
        cube->Draw(shader);
    }
    m_Plane.Draw(shader);
    m_Sphere.Draw(shader);
}

void Scene::RenderBatched(const Shader& shader, const Frustum& frustum, bool enableCulling)
{
    // Clear previous frame's batch and renderables
    m_BatchRenderer.Clear();
    m_Renderables.clear();
    m_Renderables.reserve(m_Cubes.size() + 2); // cubes + plane + sphere

    // Create renderables and store them
    for (const auto& cube : m_Cubes)
    {
        m_Renderables.push_back(cube->CreateRenderable());
    }

    m_Renderables.push_back(m_Plane.CreateRenderable());
    // Sphere doesn't have material support yet, skip it in batching

    // Submit pointers to stored renderables
    for (auto& renderable : m_Renderables)
    {
        m_BatchRenderer.Submit(&renderable);
    }

    // Prepare batches (culling happens here)
    m_BatchRenderer.Prepare(frustum, enableCulling);

    // Check if sphere is visible
    bool sphereVisible = true;
    if (enableCulling)
    {
        AABB sphereAABB = m_Sphere.GetAABB();
        sphereVisible = frustum.IsAABBVisible(sphereAABB);
    }

    // Update statistics
    m_TotalObjects = m_BatchRenderer.GetTotalRenderables() + 1; // +1 for sphere
    m_VisibleObjects = m_BatchRenderer.GetVisibleRenderables() + (sphereVisible ? 1 : 0);
    m_BatchCount = m_BatchRenderer.GetBatchCount() + (sphereVisible ? 1 : 0); // Number of material groups
    m_ActualDrawCalls = m_BatchRenderer.GetActualDrawCalls() + (sphereVisible ? 1 : 0); // Actual GPU calls
    m_DrawCalls = m_BatchCount; // For UI display, show batch count as "draw calls"

    // Render all batches
    m_BatchRenderer.Render(shader);

    // Draw sphere separately only if visible
    if (sphereVisible)
    {
        m_Sphere.Draw(shader);
    }
}

void Scene::RenderWithCullingVisualization(const Shader& shader, const Frustum& mainCameraFrustum, bool hideInvisible)
{
    shader.use();

    if (hideInvisible)
    {
        // Mode 1: Actually hide culled objects (like real culling)

        // Check and render each cube
        for (const auto& cube : m_Cubes)
        {
            AABB aabb = cube->GetAABB();
            bool isVisible = mainCameraFrustum.IsAABBVisible(aabb);

            // Skip rendering if not visible (actual culling)
            if (!isVisible)
                continue;

            // Render visible objects normally with their materials
            cube->Draw(shader);
        }

        // Check and render plane
        AABB planeAABB = m_Plane.GetAABB();
        bool planeVisible = mainCameraFrustum.IsAABBVisible(planeAABB);

        if (planeVisible)
        {
            m_Plane.Draw(shader);
        }

        // Check and render sphere
        AABB sphereAABB = m_Sphere.GetAABB();
        bool sphereVisible = mainCameraFrustum.IsAABBVisible(sphereAABB);

        if (sphereVisible)
        {
            m_Sphere.Draw(shader);
        }
    }
    else
    {
        // Mode 2: Show all objects but color-coded (green = visible, red = culled)

        // Check each cube
        for (const auto& cube : m_Cubes)
        {
            AABB aabb = cube->GetAABB();
            bool isVisible = mainCameraFrustum.IsAABBVisible(aabb);

            shader.setMat4("model", cube->GetModelMatrix());
            shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(cube->GetModelMatrix()))));

            // Color: Green if visible, Red if culled
            shader.setVec3("objectColor", isVisible ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f));

            // Set material defaults
            shader.setBool("material.useDiffuseMap", false);
            shader.setBool("material.useSpecularMap", false);
            shader.setBool("material.useNormalMap", false);
            shader.setBool("material.useEmissionMap", false);
            shader.setVec3("material.baseColor", glm::vec3(1.0f));
            shader.setFloat("material.shininess", 32.0f);
            shader.setBool("useTexture", false);

            glBindVertexArray(cube->GetVAO());
            glDrawArrays(GL_TRIANGLES, 0, cube->GetVertexCount());
            glBindVertexArray(0);
        }

        // Render plane
        AABB planeAABB = m_Plane.GetAABB();
        bool planeVisible = mainCameraFrustum.IsAABBVisible(planeAABB);

        shader.setMat4("model", m_Plane.GetModelMatrix());
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(m_Plane.GetModelMatrix()))));
        shader.setVec3("objectColor", planeVisible ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f));

        shader.setBool("material.useDiffuseMap", false);
        shader.setBool("material.useSpecularMap", false);
        shader.setBool("material.useNormalMap", false);
        shader.setBool("material.useEmissionMap", false);
        shader.setVec3("material.baseColor", glm::vec3(1.0f));
        shader.setFloat("material.shininess", 32.0f);
        shader.setBool("useTexture", false);

        glBindVertexArray(m_Plane.GetVAO());
        glDrawArrays(GL_TRIANGLES, 0, m_Plane.GetVertexCount());
        glBindVertexArray(0);

        // Draw sphere with visibility color
        AABB sphereAABB = m_Sphere.GetAABB();
        bool sphereVisible = mainCameraFrustum.IsAABBVisible(sphereAABB);
        shader.setVec3("objectColor", sphereVisible ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f));
        m_Sphere.Draw(shader);
    }
}

void Scene::RenderLightSource(const Shader& shader) const
{
    m_LightSource.Draw(shader);
}
