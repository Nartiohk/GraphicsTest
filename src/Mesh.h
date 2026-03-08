#pragma once
#include <glad/glad.h>
#include <vector>
#include <memory>

class Mesh
{
public:
    Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices = {});
    ~Mesh();

    // Disable copy, allow move
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    unsigned int GetVAO() const { return m_VAO; }
    unsigned int GetVertexCount() const { return m_VertexCount; }
    unsigned int GetIndexCount() const { return m_IndexCount; }
    bool IsIndexed() const { return m_IsIndexed; }

    // Factory methods for common shapes
    static std::shared_ptr<Mesh> CreateCube();
    static std::shared_ptr<Mesh> CreatePlane();
    static std::shared_ptr<Mesh> CreateSphere(unsigned int segments = 64);

private:
    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_EBO;
    unsigned int m_VertexCount;
    unsigned int m_IndexCount;
    bool m_IsIndexed;

    void setupMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
};
