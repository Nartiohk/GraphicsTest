#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include <vector>

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

private:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
    void setupMesh();
};