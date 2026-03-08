#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Frustum.h"
#include "BatchRenderer.h"
#include "Material.h"
#include <vector>
#include <memory>

class Sphere
{
public:
    glm::vec3 Position;
    glm::vec3 Scale;
    glm::vec3 Rotation;
    glm::vec3 Color;

    Sphere(glm::vec3 position = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 color = glm::vec3(1.0f));
    ~Sphere();

    void Draw(const Shader& shader) const;
    glm::mat4 GetModelMatrix() const;
    AABB GetAABB() const;

    // Create renderable for batch rendering
    Renderable CreateRenderable() const;

    void SetMaterial(std::shared_ptr<Material> material) { m_Material = material; }
    std::shared_ptr<Material> GetMaterial() const { return m_Material; }

    unsigned int GetVAO() const { return VAO; }
    unsigned int GetVertexCount() const { return indexCount; }
    bool IsIndexed() const { return true; }

private:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
    std::shared_ptr<Material> m_Material;
    void setupMesh();
};