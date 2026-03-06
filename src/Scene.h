#pragma once
#include <memory>
#include <vector>
#include "Cube.h"
#include "Plane.h"
#include "Sphere.h"
#include "Shader.h"

class Scene
{
public:
    Scene();
    ~Scene() = default;

    void Render(const Shader& shader) const;
    void RenderLightSource(const Shader& shader) const;

    Sphere& GetLightSource() { return m_LightSource; }
    const Sphere& GetLightSource() const { return m_LightSource; }

private:
    std::vector<std::unique_ptr<Cube>> m_Cubes;
    Plane m_Plane;
    Sphere m_Sphere;
    Sphere m_LightSource;
};
