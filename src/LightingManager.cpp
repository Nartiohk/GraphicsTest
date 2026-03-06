#include "LightingManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

LightingManager::LightingManager()
    : m_ShadowDepthShader(nullptr)
    , m_EnableShadows(true)
    , m_EnableDirLight(true)
    , m_EnablePointLight(true)
    , m_EnableSpotLight(true)
    , m_PointLightTheta(0.0f)
{
    // Create shadow maps
    m_DirLightShadowMap = std::make_unique<ShadowMap>(2048, 2048);
    m_PointLightShadowMap = std::make_unique<ShadowMap>(2048, 2048);
    m_SpotLightShadowMap = std::make_unique<ShadowMap>(2048, 2048);
}

void LightingManager::Update(float deltaTime)
{
    // Update orbiting point light
    m_PointLightTheta += deltaTime;
    m_PointLight.position.x = cos(m_PointLightTheta) * 5.0f;
    m_PointLight.position.z = sin(m_PointLightTheta) * 5.0f;
    m_PointLight.position.y = 2.0f;
}

void LightingManager::RenderShadowMaps(const std::function<void(const Shader&)>& renderSceneFunc)
{
    if (!m_ShadowDepthShader || !m_EnableShadows)
        return;

    glCullFace(GL_FRONT);

    // Render directional light shadow map
    if (m_EnableDirLight)
    {
        m_DirLightShadowMap->Bind();
        m_ShadowDepthShader->use();
        m_ShadowDepthShader->setMat4("lightSpaceMatrix", m_DirLight.GetLightSpaceMatrix());
        renderSceneFunc(*m_ShadowDepthShader);
        m_DirLightShadowMap->Unbind();
    }

    // Render point light shadow map
    if (m_EnablePointLight)
    {
        m_PointLightShadowMap->Bind();
        m_ShadowDepthShader->use();
        m_ShadowDepthShader->setMat4("lightSpaceMatrix", m_PointLight.GetLightSpaceMatrix());
        renderSceneFunc(*m_ShadowDepthShader);
        m_PointLightShadowMap->Unbind();
    }

    // Render spot light shadow map
    if (m_EnableSpotLight)
    {
        m_SpotLightShadowMap->Bind();
        m_ShadowDepthShader->use();
        m_ShadowDepthShader->setMat4("lightSpaceMatrix", m_SpotLight.GetLightSpaceMatrix());
        renderSceneFunc(*m_ShadowDepthShader);
        m_SpotLightShadowMap->Unbind();
    }

    glCullFace(GL_BACK);
}

void LightingManager::SetupLighting(const Shader& shader, const Camera& camera)
{
    shader.use();

    // Set control flags
    shader.setBool("enableShadows", m_EnableShadows);
    shader.setBool("enableDirLight", m_EnableDirLight);
    shader.setBool("enablePointLight", m_EnablePointLight);
    shader.setBool("enableSpotLight", m_EnableSpotLight);

    // Bind shadow maps
    m_DirLightShadowMap->BindTexture(0);
    shader.setInt("dirLightShadowMap", 0);

    m_PointLightShadowMap->BindTexture(1);
    shader.setInt("pointLightShadowMap", 1);

    m_SpotLightShadowMap->BindTexture(2);
    shader.setInt("spotLightShadowMap", 2);

    // Set light space matrices
    shader.setMat4("dirLightSpaceMatrix", m_DirLight.GetLightSpaceMatrix());
    shader.setMat4("pointLightSpaceMatrix", m_PointLight.GetLightSpaceMatrix());
    shader.setMat4("spotLightSpaceMatrix", m_SpotLight.GetLightSpaceMatrix());

    // Set light properties
    m_DirLight.SetUniforms(shader, "dirLight");
    m_PointLight.SetUniforms(shader, "pointLight");
    m_SpotLight.SetUniforms(shader, "spotLight");

    // Set view position
    shader.setVec3("viewPos", camera.Position);
}
