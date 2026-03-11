#pragma once
#include <vector>
#include <memory>
#include <map>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Material.h"
#include "Frustum.h"

// Renderable object interface
struct Renderable
{
    glm::mat4 modelMatrix;
    glm::mat3 normalMatrix;
    glm::vec3 color;
    std::shared_ptr<Material> material;
    AABB bounds;
    
    // Rendering data
    unsigned int VAO;
    unsigned int vertexCount;
    bool isIndexed;
    unsigned int indexCount;

    Renderable()
        : modelMatrix(1.0f)
        , normalMatrix(1.0f)
        , color(1.0f)
        , VAO(0)
        , vertexCount(0)
        , isIndexed(false)
        , indexCount(0)
    {}
};

// Batch key for grouping renderables
struct BatchKey
{
    std::shared_ptr<Material> material;
    unsigned int VAO;

    bool operator<(const BatchKey& other) const
    {
        if (material < other.material) return true;
        if (material > other.material) return false;
        return VAO < other.VAO;
    }
};

// Batch of renderables with same material and geometry
struct RenderBatch
{
    BatchKey key;
    std::vector<Renderable*> renderables;
    unsigned int drawCallCount;

    // Instance data for instanced rendering
    std::vector<glm::mat4> modelMatrices;
    std::vector<glm::mat3> normalMatrices;
    std::vector<glm::vec3> colors;
    unsigned int instanceVBO;
    bool instanceBufferInitialized;

    RenderBatch() : drawCallCount(0), instanceVBO(0), instanceBufferInitialized(false) {}
    ~RenderBatch()
    {
        if (instanceVBO != 0)
        {
            glDeleteBuffers(1, &instanceVBO);
        }
    }
};

class BatchRenderer
{
public:
    BatchRenderer();
    ~BatchRenderer() = default;

    // Add renderable to batch queue
    void Submit(Renderable* renderable);

    // Sort and batch all submitted renderables
    void Prepare(const Frustum& frustum, bool enableCulling = true);

    // Render all batches
    void Render(const Shader& shader);

    // Clear all batches
    void Clear();

    // Statistics
    unsigned int GetTotalRenderables() const { return m_TotalRenderables; }
    unsigned int GetVisibleRenderables() const { return m_VisibleRenderables; }
    unsigned int GetCulledRenderables() const { return m_TotalRenderables - m_VisibleRenderables; }
    unsigned int GetBatchCount() const;
    unsigned int GetDrawCallCount() const;
    unsigned int GetActualDrawCalls() const; // Actual GPU draw calls

private:
    std::map<BatchKey, RenderBatch> m_Batches;
    unsigned int m_TotalRenderables;
    unsigned int m_VisibleRenderables;
};
