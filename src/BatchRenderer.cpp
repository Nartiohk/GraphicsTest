#include "BatchRenderer.h"
#include <iostream>

BatchRenderer::BatchRenderer()
    : m_TotalRenderables(0)
    , m_VisibleRenderables(0)
{
}

void BatchRenderer::Submit(Renderable* renderable)
{
    if (!renderable || renderable->VAO == 0)
        return;

    BatchKey key;
    key.material = renderable->material;
    key.VAO = renderable->VAO;

    m_Batches[key].key = key;
    m_Batches[key].renderables.push_back(renderable);
    m_TotalRenderables++;
}

void BatchRenderer::Prepare(const Frustum& frustum, bool enableCulling)
{
    m_VisibleRenderables = 0;

    // Cull renderables that are outside frustum
    for (auto& batchPair : m_Batches)
    {
        auto& batch = batchPair.second;
        auto& renderables = batch.renderables;

        if (enableCulling)
        {
            // Remove culled renderables
            renderables.erase(
                std::remove_if(renderables.begin(), renderables.end(),
                    [&frustum, this](Renderable* r) {
                        bool visible = frustum.IsAABBVisible(r->bounds);
                        if (visible) m_VisibleRenderables++;
                        return !visible;
                    }),
                renderables.end()
            );
        }
        else
        {
            m_VisibleRenderables += renderables.size();
        }

        batch.drawCallCount = renderables.size();
    }
}

void BatchRenderer::Render(const Shader& shader)
{
    for (auto& batchPair : m_Batches)
    {
        auto& batch = batchPair.second;
        
        if (batch.renderables.empty())
            continue;

        // Bind material once for all renderables in batch
        if (batch.key.material)
        {
            batch.key.material->Bind(shader);
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

        // Bind VAO once for all renderables in batch
        glBindVertexArray(batch.key.VAO);

        // Draw all renderables in this batch
        for (Renderable* renderable : batch.renderables)
        {
            // Set per-object uniforms
            shader.setMat4("model", renderable->modelMatrix);
            shader.setMat3("normalMatrix", renderable->normalMatrix);
            shader.setVec3("objectColor", renderable->color);

            // Draw
            if (renderable->isIndexed)
            {
                glDrawElements(GL_TRIANGLES, renderable->indexCount, GL_UNSIGNED_INT, 0);
            }
            else
            {
                glDrawArrays(GL_TRIANGLES, 0, renderable->vertexCount);
            }
        }

        glBindVertexArray(0);
    }
}

void BatchRenderer::Clear()
{
    m_Batches.clear();
    m_TotalRenderables = 0;
    m_VisibleRenderables = 0;
}

unsigned int BatchRenderer::GetDrawCallCount() const
{
    unsigned int count = 0;
    for (const auto& batchPair : m_Batches)
    {
        count += batchPair.second.drawCallCount;
    }
    return count;
}
