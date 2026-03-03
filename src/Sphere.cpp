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
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
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

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) std::cout << "Error after setMat4: " << err << std::endl;

    shader.setVec3("objectColor", Color);
    glBindVertexArray(VAO);
    err = glGetError();
    if (err != GL_NO_ERROR) std::cout << "Error after glBindVertexArray: " << err << std::endl;

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    err = glGetError();
    if (err != GL_NO_ERROR) std::cout << "Error after glDrawElements: " << err << std::endl;

    glBindVertexArray(0);
    err = glGetError();
    if (err != GL_NO_ERROR) std::cout << "Error after unbind VAO: " << err << std::endl;
}