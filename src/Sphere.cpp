#include "Sphere.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>

Sphere::Sphere(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, glm::vec3 color)
    : Position(position), 
      Scale(scale), 
      Rotation(rotation),
      Color(color)
{
    setupMesh();
}

Sphere::~Sphere()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Sphere::setupMesh()
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = glm::pi<float>();
    const float textureRepeat = 4.0f; // Repeat texture 4 times for better detail

    // Generate vertices
    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
    {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            // Position
            vertices.push_back(xPos * 0.5f);
            vertices.push_back(yPos * 0.5f);
            vertices.push_back(zPos * 0.5f);

            // Normal (for a sphere centered at origin, normal = normalized position)
            glm::vec3 normal(xPos, yPos, zPos);
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);

            // Texture coordinates (repeated for better detail)
            vertices.push_back(xSegment * textureRepeat);
            vertices.push_back(ySegment * textureRepeat);

            // Calculate tangent for normal mapping
            // Tangent points in the direction of increasing U (horizontal)
            glm::vec3 tangent;
            tangent.x = -std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            tangent.y = 0.0f;
            tangent.z = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            tangent = glm::normalize(tangent);

            vertices.push_back(tangent.x);
            vertices.push_back(tangent.y);
            vertices.push_back(tangent.z);

            // Calculate bitangent (perpendicular to both normal and tangent)
            glm::vec3 bitangent = glm::cross(normal, tangent);
            bitangent = glm::normalize(bitangent);

            vertices.push_back(bitangent.x);
            vertices.push_back(bitangent.y);
            vertices.push_back(bitangent.z);
        }
    }

    // Generate indices
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        for (unsigned int x = 0; x < X_SEGMENTS; ++x)
        {
            unsigned int p1 = y * (X_SEGMENTS + 1) + x;
            unsigned int p2 = p1 + X_SEGMENTS + 1;

            indices.push_back(p1);
            indices.push_back(p2);
            indices.push_back(p1 + 1);

            indices.push_back(p1 + 1);
            indices.push_back(p2);
            indices.push_back(p2 + 1);
        }
    }

    indexCount = indices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute (location 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Tangent attribute (location 3)
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    // Bitangent attribute (location 4)
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
}

glm::mat4 Sphere::GetModelMatrix() const
{
    glm::mat4 model = glm::mat4(1.0f);
    
    model = glm::translate(model, Position);
    
    model = glm::rotate(model, glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    
    model = glm::scale(model, Scale);
    
    return model;
}

void Sphere::Draw(const Shader& shader) const
{
    glm::mat4 model = GetModelMatrix();
    shader.setMat4("model", model);

    // Calculate normal matrix (transpose of inverse of upper-left 3x3 of model matrix)
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

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

AABB Sphere::GetAABB() const
{
    // Sphere AABB is a cube that contains the sphere
    // For a unit sphere, this is [-1, -1, -1] to [1, 1, 1]
    AABB aabb(glm::vec3(-1.0f), glm::vec3(1.0f));

    // Transform by model matrix
    return aabb.Transform(GetModelMatrix());
}

Renderable Sphere::CreateRenderable() const
{
    Renderable r;
    r.modelMatrix = GetModelMatrix();
    r.normalMatrix = glm::transpose(glm::inverse(glm::mat3(r.modelMatrix)));
    r.color = Color;
    r.material = m_Material;
    r.bounds = GetAABB();
    r.VAO = VAO;
    r.vertexCount = 0; // Not used for indexed rendering
    r.isIndexed = true;
    r.indexCount = indexCount;
    return r;
}
