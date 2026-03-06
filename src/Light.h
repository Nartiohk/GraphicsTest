#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

struct DirectionalLight
{
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    // Intensity multipliers
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;

    DirectionalLight()
        : direction(-0.2f, -1.0f, -0.3f)
        , ambient(0.05f, 0.05f, 0.05f)
        , diffuse(0.4f, 0.4f, 0.4f)
        , specular(0.5f, 0.5f, 0.5f)
        , ambientIntensity(1.0f)
        , diffuseIntensity(1.0f)
        , specularIntensity(1.0f)
    {}

    glm::mat4 GetLightSpaceMatrix() const
    {
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
        glm::mat4 lightView = glm::lookAt(
            -direction * 10.0f,
            glm::vec3(0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        return lightProjection * lightView;
    }

    void SetUniforms(const Shader& shader, const std::string& name) const
    {
        shader.setVec3(name + ".direction", direction);
        shader.setVec3(name + ".ambient", ambient * ambientIntensity);
        shader.setVec3(name + ".diffuse", diffuse * diffuseIntensity);
        shader.setVec3(name + ".specular", specular * specularIntensity);
    }
};

struct PointLight
{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;

    // Intensity multipliers
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;

    PointLight()
        : position(0.0f, 2.0f, 0.0f)
        , ambient(0.05f, 0.05f, 0.05f)
        , diffuse(0.8f, 0.8f, 0.8f)
        , specular(1.0f, 1.0f, 1.0f)
        , constant(1.0f)
        , linear(0.09f)
        , quadratic(0.032f)
        , ambientIntensity(1.0f)
        , diffuseIntensity(1.0f)
        , specularIntensity(1.0f)
    {}

    glm::mat4 GetLightSpaceMatrix() const
    {
        glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 25.0f);
        glm::mat4 lightView = glm::lookAt(
            position,
            glm::vec3(0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        return lightProjection * lightView;
    }

    void SetUniforms(const Shader& shader, const std::string& name) const
    {
        shader.setVec3(name + ".position", position);
        shader.setVec3(name + ".ambient", ambient * ambientIntensity);
        shader.setVec3(name + ".diffuse", diffuse * diffuseIntensity);
        shader.setVec3(name + ".specular", specular * specularIntensity);
        shader.setFloat(name + ".constant", constant);
        shader.setFloat(name + ".linear", linear);
        shader.setFloat(name + ".quadratic", quadratic);
    }
};

struct SpotLight
{
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;

    // Intensity multipliers
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;

    SpotLight()
        : position(0.0f, 0.0f, 0.0f)
        , direction(0.0f, 0.0f, -1.0f)
        , ambient(0.0f, 0.0f, 0.0f)
        , diffuse(1.0f, 1.0f, 1.0f)
        , specular(1.0f, 1.0f, 1.0f)
        , constant(1.0f)
        , linear(0.09f)
        , quadratic(0.032f)
        , cutOff(glm::cos(glm::radians(12.5f)))
        , outerCutOff(glm::cos(glm::radians(15.0f)))
        , ambientIntensity(1.0f)
        , diffuseIntensity(1.0f)
        , specularIntensity(1.0f)
    {}

    glm::mat4 GetLightSpaceMatrix() const
    {
        glm::mat4 lightProjection = glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 25.0f);
        glm::mat4 lightView = glm::lookAt(
            position,
            position + direction,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        return lightProjection * lightView;
    }

    void SetUniforms(const Shader& shader, const std::string& name) const
    {
        shader.setVec3(name + ".position", position);
        shader.setVec3(name + ".direction", direction);
        shader.setVec3(name + ".ambient", ambient * ambientIntensity);
        shader.setVec3(name + ".diffuse", diffuse * diffuseIntensity);
        shader.setVec3(name + ".specular", specular * specularIntensity);
        shader.setFloat(name + ".constant", constant);
        shader.setFloat(name + ".linear", linear);
        shader.setFloat(name + ".quadratic", quadratic);
        shader.setFloat(name + ".cutOff", cutOff);
        shader.setFloat(name + ".outerCutOff", outerCutOff);
    }
};
