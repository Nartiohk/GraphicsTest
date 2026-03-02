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
    float vertices[] = {
        // Positions          // Normals
        -0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,
         0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,
        -0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,
        -0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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

void Plane::Draw(const Shader& shader) const
{
    shader.setMat4("model", GetModelMatrix());
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) std::cout << "Error after setMat4: " << err << std::endl;

	shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(GetModelMatrix()))));
	err = glGetError();
	if (err != GL_NO_ERROR) std::cout << "Error after setMat3: " << err << std::endl;

    shader.setVec3("objectColor", Color);
    glBindVertexArray(VAO);
    err = glGetError();
    if (err != GL_NO_ERROR) std::cout << "Error after glBindVertexArray: " << err << std::endl;

    glDrawArrays(GL_TRIANGLES, 0, 6);
    err = glGetError();
    if (err != GL_NO_ERROR) std::cout << "Error after glDrawArrays: " << err << std::endl;

    glBindVertexArray(0);
    err = glGetError();
    if (err != GL_NO_ERROR) std::cout << "Error after unbind VAO: " << err << std::endl;
}
