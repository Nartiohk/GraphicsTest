#include "Material.h"
#include <glm/glm.hpp>

Material::Material()
    : m_Shininess(32.0f)
    , m_BaseColor(1.0f, 1.0f, 1.0f)
{
}

void Material::SetDiffuseTexture(std::shared_ptr<Texture> texture)
{
    m_DiffuseTexture = texture;
}

void Material::SetSpecularTexture(std::shared_ptr<Texture> texture)
{
    m_SpecularTexture = texture;
}

void Material::SetNormalTexture(std::shared_ptr<Texture> texture)
{
    m_NormalTexture = texture;
}

void Material::SetEmissionTexture(std::shared_ptr<Texture> texture)
{
    m_EmissionTexture = texture;
}

void Material::Bind(const Shader& shader) const
{
    // Base color
    shader.setVec3("material.baseColor", m_BaseColor);
    shader.setFloat("material.shininess", m_Shininess);

    // Diffuse texture (Unit 3)
    if (HasDiffuseTexture())
    {
        shader.setBool("material.useDiffuseMap", true);
        m_DiffuseTexture->Bind(3);
        shader.setInt("material.diffuseMap", 3);
    }
    else
    {
        shader.setBool("material.useDiffuseMap", false);
    }

    // Specular texture (Unit 4)
    if (HasSpecularTexture())
    {
        shader.setBool("material.useSpecularMap", true);
        m_SpecularTexture->Bind(4);
        shader.setInt("material.specularMap", 4);
    }
    else
    {
        shader.setBool("material.useSpecularMap", false);
    }

    // Normal texture (Unit 5)
    if (HasNormalTexture())
    {
        shader.setBool("material.useNormalMap", true);
        m_NormalTexture->Bind(5);
        shader.setInt("material.normalMap", 5);
    }
    else
    {
        shader.setBool("material.useNormalMap", false);
    }

    // Emission texture (Unit 6)
    if (HasEmissionTexture())
    {
        shader.setBool("material.useEmissionMap", true);
        m_EmissionTexture->Bind(6);
        shader.setInt("material.emissionMap", 6);
    }
    else
    {
        shader.setBool("material.useEmissionMap", false);
    }
}
