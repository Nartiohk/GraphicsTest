#pragma once
#include <memory>
#include <string>
#include "Texture.h"
#include "Shader.h"

enum class TextureType
{
    Diffuse,
    Specular,
    Normal,
    Emission
};

class Material
{
public:
    Material();
    ~Material() = default;

    // Set textures
    void SetDiffuseTexture(std::shared_ptr<Texture> texture);
    void SetSpecularTexture(std::shared_ptr<Texture> texture);
    void SetNormalTexture(std::shared_ptr<Texture> texture);
    void SetEmissionTexture(std::shared_ptr<Texture> texture);

    // Get textures
    std::shared_ptr<Texture> GetDiffuseTexture() const { return m_DiffuseTexture; }
    std::shared_ptr<Texture> GetSpecularTexture() const { return m_SpecularTexture; }
    std::shared_ptr<Texture> GetNormalTexture() const { return m_NormalTexture; }
    std::shared_ptr<Texture> GetEmissionTexture() const { return m_EmissionTexture; }

    // Material properties
    void SetShininess(float shininess) { m_Shininess = shininess; }
    float GetShininess() const { return m_Shininess; }

    void SetBaseColor(const glm::vec3& color) { m_BaseColor = color; }
    glm::vec3 GetBaseColor() const { return m_BaseColor; }

    // Check what textures are available
    bool HasDiffuseTexture() const { return m_DiffuseTexture && m_DiffuseTexture->IsLoaded(); }
    bool HasSpecularTexture() const { return m_SpecularTexture && m_SpecularTexture->IsLoaded(); }
    bool HasNormalTexture() const { return m_NormalTexture && m_NormalTexture->IsLoaded(); }
    bool HasEmissionTexture() const { return m_EmissionTexture && m_EmissionTexture->IsLoaded(); }

    // Bind all material textures and set shader uniforms
    void Bind(const Shader& shader) const;

private:
    std::shared_ptr<Texture> m_DiffuseTexture;
    std::shared_ptr<Texture> m_SpecularTexture;
    std::shared_ptr<Texture> m_NormalTexture;
    std::shared_ptr<Texture> m_EmissionTexture;

    float m_Shininess;
    glm::vec3 m_BaseColor;
};
