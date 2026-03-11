#include "Scene.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Scene::Scene()
    : m_TotalObjects(0)
    , m_VisibleObjects(0)
    , m_DrawCalls(0)
    , m_BatchCount(0)
    , m_ActualDrawCalls(0)
{
    // Load textures and create materials
    LoadTextures();
    CreateMaterials();

    // Create shared meshes (created once, shared by all objects)
    CreateMeshes();

    // Create object instances (each references a shared mesh)
    CreateObjects();
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
    // Update statistics for non-batched rendering ONLY if this is the main render path
    // Don't overwrite batch statistics when called from mini-map
    if (m_BatchRenderer.GetBatchCount() == 0)
    {
        // Main view is using non-batched rendering, update stats
        m_TotalObjects = m_Objects.size();
        m_VisibleObjects = m_TotalObjects;
        m_DrawCalls = m_TotalObjects;
        m_BatchCount = 0;
        m_ActualDrawCalls = m_TotalObjects;
    }

    for (const auto& object : m_Objects)
    {
        object->Draw(shader);
    }
}

void Scene::RenderBatched(const Shader& shader, const Frustum& frustum, bool enableCulling)
{
    // Clear previous frame's batch and renderables
    m_BatchRenderer.Clear();
    m_Renderables.clear();
    m_Renderables.reserve(m_Objects.size());

    // Create renderables and store them
    for (const auto& object : m_Objects)
    {
        m_Renderables.push_back(object->CreateRenderable());
    }

    // Submit pointers to stored renderables
    for (auto& renderable : m_Renderables)
    {
        m_BatchRenderer.Submit(&renderable);
    }

    // Prepare batches (culling happens here)
    m_BatchRenderer.Prepare(frustum, enableCulling);

    // Update statistics
    m_TotalObjects = m_BatchRenderer.GetTotalRenderables();
    m_VisibleObjects = m_BatchRenderer.GetVisibleRenderables();
    m_BatchCount = m_BatchRenderer.GetBatchCount();
    m_ActualDrawCalls = m_BatchRenderer.GetActualDrawCalls();
    m_DrawCalls = m_BatchCount; // For UI display, show batch count as "draw calls"

    // Render all batches
    m_BatchRenderer.Render(shader);
}

void Scene::RenderWithCullingVisualization(const Shader& shader, const Frustum& mainCameraFrustum, bool hideInvisible)
{
    shader.use();

    if (hideInvisible)
    {
        // Mode 1: Actually hide culled objects (like real culling)

        for (const auto& object : m_Objects)
        {
            AABB aabb = object->GetAABB();
            bool isVisible = mainCameraFrustum.IsAABBVisible(aabb);

            // Skip rendering if not visible (actual culling)
            if (!isVisible)
                continue;

            // Render visible objects normally with their materials
            object->Draw(shader);
        }
    }
    else
    {
        // Mode 2: Show all objects but color-coded (green = visible, red = culled)

        for (const auto& object : m_Objects)
        {
            AABB aabb = object->GetAABB();
            bool isVisible = mainCameraFrustum.IsAABBVisible(aabb);

            shader.setMat4("model", object->GetModelMatrix());
            shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(object->GetModelMatrix()))));

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

            glBindVertexArray(object->GetMesh()->GetVAO());

            if (object->GetMesh()->IsIndexed())
            {
                glDrawElements(GL_TRIANGLES, object->GetMesh()->GetIndexCount(), GL_UNSIGNED_INT, 0);
            }
            else
            {
                glDrawArrays(GL_TRIANGLES, 0, object->GetMesh()->GetVertexCount());
            }

            glBindVertexArray(0);
        }
    }
}

void Scene::RenderLightSource(const Shader& shader) const
{
    m_LightSource->Draw(shader);
}

void Scene::CreateMeshes()
{
    std::cout << "Creating shared meshes..." << std::endl;

    // Create meshes once - will be shared by all object instances
    m_CubeMesh = Mesh::CreateCube();
    m_PlaneMesh = Mesh::CreatePlane();
    m_SphereMesh = Mesh::CreateSphere(64); // 64x64 segments

    std::cout << "Meshes created (1 cube, 1 plane, 1 sphere)" << std::endl;
}

void Scene::CreateObjects()
{
    std::cout << "Creating object instances..." << std::endl;

    glm::vec3 cubeColor(0.0f, 0.5f, 0.31f);

    // Create 5 cube instances in a circle (all sharing same cube mesh!)
    for (int i = 0; i < 5; ++i)
    {
        float angle = (float)i / 5.0f * 360.0f;
        float radius = 3.0f;
        float x = cos(glm::radians(angle)) * radius;
        float z = sin(glm::radians(angle)) * radius;

        auto cube = std::make_unique<Object>(
            m_CubeMesh,                      // Shared mesh!
            glm::vec3(x, 0.0f, z),          // Position
            glm::vec3(1.0f),                // Scale
            glm::vec3(0.0f),                // Rotation
            cubeColor                        // Color
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

        m_Objects.push_back(std::move(cube));
    }

    // Create plane (shares plane mesh)
    auto plane = std::make_unique<Object>(
        m_PlaneMesh,                        // Shared mesh!
        glm::vec3(0.0f, -1.0f, 0.0f),      // Position
        glm::vec3(10.0f, 1.0f, 10.0f),     // Scale
        glm::vec3(0.0f),                    // Rotation
        glm::vec3(0.3f, 0.3f, 0.3f)        // Color
    );
    plane->SetMaterial(m_BrickMaterial);
    m_Objects.push_back(std::move(plane));

    // Create center sphere (shares sphere mesh)
    auto sphere = std::make_unique<Object>(
        m_SphereMesh,                       // Shared mesh!
        glm::vec3(0.0f, 0.0f, 0.0f),       // Position
        glm::vec3(2.0f),                    // Scale
        glm::vec3(0.0f),                    // Rotation
        glm::vec3(1.0f, 0.2f, 0.3f)        // Color
    );
    sphere->SetMaterial(m_BrickMaterial);
    m_Objects.push_back(std::move(sphere));

    // Create light source sphere (shares sphere mesh!)
    m_LightSource = std::make_unique<Object>(
        m_SphereMesh,                       // Shared mesh!
        glm::vec3(0.0f, 2.0f, 0.0f),       // Position
        glm::vec3(0.1f),                    // Scale
        glm::vec3(0.0f),                    // Rotation
        glm::vec3(1.0f, 1.0f, 1.0f)        // Color
    );

    std::cout << "Objects created (" << m_Objects.size() + 1 << " total)" << std::endl;
    std::cout << "Memory savings: All cubes share 1 mesh, both spheres share 1 mesh!" << std::endl;
}
