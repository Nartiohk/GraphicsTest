#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Frustum.h"
#include "BatchRenderer.h"

class Object
{
public:
    // Transform
    glm::vec3 Position;
    glm::vec3 Scale;
    glm::vec3 Rotation; // Euler angles in degrees
    glm::vec3 Color;

    Object(std::shared_ptr<Mesh> mesh, 
           glm::vec3 position = glm::vec3(0.0f), 
           glm::vec3 scale = glm::vec3(1.0f),
           glm::vec3 rotation = glm::vec3(0.0f),
           glm::vec3 color = glm::vec3(1.0f));

    void Draw(const Shader& shader) const;
    glm::mat4 GetModelMatrix() const;
    AABB GetAABB() const;
    Renderable CreateRenderable() const;

    void SetMaterial(std::shared_ptr<Material> material) { m_Material = material; }
    std::shared_ptr<Material> GetMaterial() const { return m_Material; }

    std::shared_ptr<Mesh> GetMesh() const { return m_Mesh; }

private:
    std::shared_ptr<Mesh> m_Mesh;
    std::shared_ptr<Material> m_Material;
};
