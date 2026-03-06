#pragma once
#include <memory>
#include <vector>
#include <functional>
#include "Light.h"
#include "ShadowMap.h"
#include "Shader.h"
#include "Camera.h"

class LightingManager
{
public:
    LightingManager();
    ~LightingManager() = default;

    void Update(float deltaTime);
    void RenderShadowMaps(const std::function<void(const Shader&)>& renderSceneFunc);
    void SetupLighting(const Shader& shader, const Camera& camera);

    DirectionalLight& GetDirectionalLight() { return m_DirLight; }
    PointLight& GetPointLight() { return m_PointLight; }
    SpotLight& GetSpotLight() { return m_SpotLight; }

    void SetEnableShadows(bool enable) { m_EnableShadows = enable; }
    void SetEnableDirLight(bool enable) { m_EnableDirLight = enable; }
    void SetEnablePointLight(bool enable) { m_EnablePointLight = enable; }
    void SetEnableSpotLight(bool enable) { m_EnableSpotLight = enable; }

    bool IsEnableShadows() const { return m_EnableShadows; }
    bool IsEnableDirLight() const { return m_EnableDirLight; }
    bool IsEnablePointLight() const { return m_EnablePointLight; }
    bool IsEnableSpotLight() const { return m_EnableSpotLight; }

    const ShadowMap& GetDirLightShadowMap() const { return *m_DirLightShadowMap; }
    const ShadowMap& GetPointLightShadowMap() const { return *m_PointLightShadowMap; }
    const ShadowMap& GetSpotLightShadowMap() const { return *m_SpotLightShadowMap; }

    void SetShadowDepthShader(Shader* shader) { m_ShadowDepthShader = shader; }

private:
    DirectionalLight m_DirLight;
    PointLight m_PointLight;
    SpotLight m_SpotLight;

    std::unique_ptr<ShadowMap> m_DirLightShadowMap;
    std::unique_ptr<ShadowMap> m_PointLightShadowMap;
    std::unique_ptr<ShadowMap> m_SpotLightShadowMap;

    Shader* m_ShadowDepthShader;

    bool m_EnableShadows;
    bool m_EnableDirLight;
    bool m_EnablePointLight;
    bool m_EnableSpotLight;

    float m_PointLightTheta;
};
