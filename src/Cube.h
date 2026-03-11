#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include "Shader.h"
#include "Material.h"
#include "Frustum.h"
#include "BatchRenderer.h"

class Cube
{
public:
    glm::vec3 Position;
    glm::vec3 Scale;
    glm::vec3 Rotation;
    glm::vec3 Color;

    Cube(glm::vec3 position = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 color = glm::vec3(1.0f));
    ~Cube();

    void Draw(const Shader& shader) const;
    glm::mat4 GetModelMatrix() const;
    AABB GetAABB() const;

    // Create renderable for batch rendering
    Renderable CreateRenderable() const;

    void SetMaterial(std::shared_ptr<Material> material) { m_Material = material; }
    std::shared_ptr<Material> GetMaterial() const { return m_Material; }

    // Keep texture support for backward compatibility
    void SetTexture(std::shared_ptr<Texture> texture);
    std::shared_ptr<Texture> GetTexture() const;

    unsigned int GetVAO() const { return VAO; }
    unsigned int GetVertexCount() const { return 36; }

private:
    unsigned int VAO, VBO;
    std::shared_ptr<Material> m_Material;
    void setupMesh();
};
