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
    void RenderWithCullingVisualization(const Shader& shader, const Frustum& mainCameraFrustum, bool hideInvisible = true);
    void RenderLightSource(const Shader& shader) const;

    Sphere& GetLightSource() { return m_LightSource; }
    const Sphere& GetLightSource() const { return m_LightSource; }

    // Statistics (works for both batched and non-batched)
    unsigned int GetTotalObjects() const { return m_TotalObjects; }
    unsigned int GetVisibleObjects() const { return m_VisibleObjects; }
    unsigned int GetCulledObjects() const { return m_TotalObjects - m_VisibleObjects; }
    unsigned int GetDrawCalls() const { return m_DrawCalls; }
    unsigned int GetBatchCount() const { return m_BatchCount; }
    unsigned int GetActualDrawCalls() const { return m_ActualDrawCalls; }

private:
    std::vector<std::unique_ptr<Cube>> m_Cubes;
    Plane m_Plane;
    Sphere m_Sphere;
    Sphere m_LightSource;

    // Materials
    std::shared_ptr<Material> m_BrickMaterial;
    std::shared_ptr<Material> m_ContainerMaterial;

    // Batch renderer and renderable storage
    mutable BatchRenderer m_BatchRenderer;
    mutable std::vector<Renderable> m_Renderables;

    // Statistics
    mutable unsigned int m_TotalObjects;
    mutable unsigned int m_VisibleObjects;
    mutable unsigned int m_DrawCalls;
    mutable unsigned int m_BatchCount;
    mutable unsigned int m_ActualDrawCalls;

    void LoadTextures();
    void CreateMaterials();
};
