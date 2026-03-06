#pragma once
#include <memory>
#include <vector>
#include "Cube.h"
#include "Plane.h"
#include "Sphere.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "BatchRenderer.h"
#include "Frustum.h"

class Scene
{
public:
    Scene();
    ~Scene() = default;

    void Render(const Shader& shader) const;
    void RenderBatched(const Shader& shader, const Frustum& frustum, bool enableCulling = true);
    void RenderLightSource(const Shader& shader) const;

    Sphere& GetLightSource() { return m_LightSource; }
    const Sphere& GetLightSource() const { return m_LightSource; }

    // Batching statistics
    unsigned int GetTotalObjects() const;
    unsigned int GetVisibleObjects() const;
    unsigned int GetCulledObjects() const;
    unsigned int GetDrawCalls() const;

private:
    std::vector<std::unique_ptr<Cube>> m_Cubes;
    Plane m_Plane;
    Sphere m_Sphere;
    Sphere m_LightSource;

    // Materials
    std::shared_ptr<Material> m_BrickMaterial;
    std::shared_ptr<Material> m_ContainerMaterial;

    // Batch renderer
    mutable BatchRenderer m_BatchRenderer;

    void LoadTextures();
    void CreateMaterials();
};
