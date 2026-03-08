#include "Object.h"
#include <glm/gtc/matrix_transform.hpp>

Object::Object(std::shared_ptr<Mesh> mesh, 
               glm::vec3 position, 
               glm::vec3 scale, 
               glm::vec3 rotation, 
               glm::vec3 color)
    : Position(position)
    , Scale(scale)
    , Rotation(rotation)
    , Color(color)
    , m_Mesh(mesh)
    , m_Material(nullptr)
{
}

glm::mat4 Object::GetModelMatrix() const
{
    glm::mat4 model = glm::mat4(1.0f);
    
    model = glm::translate(model, Position);
    
    model = glm::rotate(model, glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    
    model = glm::scale(model, Scale);
    
    return model;
}

AABB Object::GetAABB() const
{
    // Base AABB (for unit cube/sphere/plane)
    AABB aabb(glm::vec3(-0.5f), glm::vec3(0.5f));
    
    // Transform by model matrix
    return aabb.Transform(GetModelMatrix());
}

void Object::Draw(const Shader& shader) const
{
    glm::mat4 model = GetModelMatrix();
    shader.setMat4("model", model);

    // Calculate normal matrix
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
    shader.setMat3("normalMatrix", normalMatrix);

    shader.setVec3("objectColor", Color);

    // Bind material if available
    if (m_Material)
    {
        m_Material->Bind(shader);
    }
    else
    {
        // Set defaults if no material
        shader.setBool("material.useDiffuseMap", false);
        shader.setBool("material.useSpecularMap", false);
        shader.setBool("material.useNormalMap", false);
        shader.setBool("material.useEmissionMap", false);
        shader.setVec3("material.baseColor", glm::vec3(1.0f));
        shader.setFloat("material.shininess", 32.0f);
        shader.setBool("useTexture", false);
    }

    // Draw mesh
    glBindVertexArray(m_Mesh->GetVAO());
    if (m_Mesh->IsIndexed())
    {
        glDrawElements(GL_TRIANGLES, m_Mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, m_Mesh->GetVertexCount());
    }
    glBindVertexArray(0);
}

Renderable Object::CreateRenderable() const
{
    Renderable r;
    r.modelMatrix = GetModelMatrix();
    r.normalMatrix = glm::transpose(glm::inverse(glm::mat3(r.modelMatrix)));
    r.color = Color;
    r.material = m_Material;
    r.bounds = GetAABB();
    r.VAO = m_Mesh->GetVAO();
    r.vertexCount = m_Mesh->GetVertexCount();
    r.isIndexed = m_Mesh->IsIndexed();
    r.indexCount = m_Mesh->GetIndexCount();
    return r;
}
