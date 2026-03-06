#include "Scene.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Scene::Scene()
    : m_Plane(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(10.0f, 1.0f, 10.0f), glm::vec3(0.0f), glm::vec3(0.3f, 0.3f, 0.3f))
    , m_Sphere(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f), glm::vec3(0.0f), glm::vec3(1.0f, 0.2f, 0.3f))
    , m_LightSource(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.1f), glm::vec3(0.0f), glm::vec3(1.0f, 1.0f, 1.0f))
{
    // Create multiple cube instances in a circle
    glm::vec3 cubeColor(0.0f, 0.5f, 0.31f);
    for (int i = 0; i < 5; ++i)
    {
        float angle = (float)i / 5.0f * 360.0f;
        float radius = 3.0f;
        float x = cos(glm::radians(angle)) * radius;
        float z = sin(glm::radians(angle)) * radius;
        m_Cubes.push_back(std::make_unique<Cube>(
            glm::vec3(x, 0.0f, z),
            glm::vec3(1.0f),
            glm::vec3(0.0f),
            cubeColor
        ));
    }
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

void Scene::RenderLightSource(const Shader& shader) const
{
    m_LightSource.Draw(shader);
}
