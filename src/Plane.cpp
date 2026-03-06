#include "Plane.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Plane::Plane(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, glm::vec3 color)
    : Position(position), 
      Scale(scale), 
      Rotation(rotation),
      Color(color)
{
    setupMesh();
}

Plane::~Plane()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Plane::setupMesh()
{
    // Position (3), Normal (3), TexCoord (2), Tangent (3), Bitangent (3) = 14 floats per vertex
    // Normal: (0,1,0), Tangent: (1,0,0), Bitangent: (0,0,1)
    float vertices[] = {
        // Positions          // Normals           // TexCoords  // Tangent         // Bitangent
        -0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
         0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,   10.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
         0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,   10.0f, 10.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
         0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,   10.0f, 10.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 10.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Tangent attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    // Bitangent attribute
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
}

glm::mat4 Plane::GetModelMatrix() const
{
    glm::mat4 model = glm::mat4(1.0f);
    
    model = glm::translate(model, Position);
    
    model = glm::rotate(model, glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    
    model = glm::scale(model, Scale);
    
    return model;
}

AABB Plane::GetAABB() const
{
    // Base plane AABB (unit square on XZ plane)
    AABB aabb(glm::vec3(-0.5f, -0.01f, -0.5f), glm::vec3(0.5f, 0.01f, 0.5f));

    // Transform by model matrix
    return aabb.Transform(GetModelMatrix());
}

Renderable Plane::CreateRenderable() const
{
    Renderable r;
    r.modelMatrix = GetModelMatrix();
    r.normalMatrix = glm::transpose(glm::inverse(glm::mat3(r.modelMatrix)));
    r.color = Color;
    r.material = m_Material;
    r.bounds = GetAABB();
    r.VAO = VAO;
    r.vertexCount = 6;
    r.isIndexed = false;
    return r;
}

void Plane::Draw(const Shader& shader) const
{
	shader.setMat4("model", GetModelMatrix());
	shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(GetModelMatrix()))));
	shader.setVec3("objectColor", Color);

	// Use material if available
	if (m_Material)
	{
		m_Material->Bind(shader);
	}
	else
	{
		// Set defaults for no material
		shader.setBool("material.useDiffuseMap", false);
		shader.setBool("material.useSpecularMap", false);
		shader.setBool("material.useNormalMap", false);
		shader.setBool("material.useEmissionMap", false);
		shader.setVec3("material.baseColor", glm::vec3(1.0f));
		shader.setFloat("material.shininess", 32.0f);
		shader.setBool("useTexture", false);
	}

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

// Backward compatibility for texture
void Plane::SetTexture(std::shared_ptr<Texture> texture)
{
	if (!m_Material)
	{
		m_Material = std::make_shared<Material>();
	}
	m_Material->SetDiffuseTexture(texture);
}

std::shared_ptr<Texture> Plane::GetTexture() const
{
	if (m_Material)
	{
		return m_Material->GetDiffuseTexture();
	}
	return nullptr;
}
