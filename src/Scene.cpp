#include "Scene.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Scene::Scene()
    : m_Plane(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(10.0f, 1.0f, 10.0f), glm::vec3(0.0f), glm::vec3(0.3f, 0.3f, 0.3f))
    , m_Sphere(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f), glm::vec3(0.0f), glm::vec3(1.0f, 0.2f, 0.3f))
    , m_LightSource(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.1f), glm::vec3(0.0f), glm::vec3(1.0f, 1.0f, 1.0f))
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
    for (const auto& cube : m_Cubes)
    {
        cube->Draw(shader);
    }
    m_Plane.Draw(shader);
    m_Sphere.Draw(shader);
}

void Scene::RenderBatched(const Shader& shader, const Frustum& frustum, bool enableCulling)
{
    // Clear previous frame's batch
    m_BatchRenderer.Clear();

    // Submit all renderables to batch renderer
    for (const auto& cube : m_Cubes)
    {
        Renderable r = cube->CreateRenderable();
        m_BatchRenderer.Submit(&r);
    }

    Renderable planeR = m_Plane.CreateRenderable();
    m_BatchRenderer.Submit(&planeR);

    // Sphere doesn't have material/batching support yet, draw separately

    // Prepare batches (culling happens here)
    m_BatchRenderer.Prepare(frustum, enableCulling);

    // Render all batches
    m_BatchRenderer.Render(shader);

    // Draw sphere separately (not batched)
    m_Sphere.Draw(shader);
}

unsigned int Scene::GetTotalObjects() const
{
    return m_BatchRenderer.GetTotalRenderables();
}

unsigned int Scene::GetVisibleObjects() const
{
    return m_BatchRenderer.GetVisibleRenderables();
}

unsigned int Scene::GetCulledObjects() const
{
    return m_BatchRenderer.GetCulledRenderables();
}

unsigned int Scene::GetDrawCalls() const
{
    return m_BatchRenderer.GetDrawCallCount() + 1; // +1 for sphere
}

void Scene::RenderLightSource(const Shader& shader) const
{
    m_LightSource.Draw(shader);
}
