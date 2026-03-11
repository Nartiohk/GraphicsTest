#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

// Axis-Aligned Bounding Box
struct AABB
{
    glm::vec3 min;
    glm::vec3 max;

    AABB() : min(0.0f), max(0.0f) {}
    AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

    // Get center of AABB
    glm::vec3 GetCenter() const { return (min + max) * 0.5f; }
    
    // Get extents (half-sizes)
    glm::vec3 GetExtents() const { return (max - min) * 0.5f; }
    
    // Transform AABB by model matrix
    AABB Transform(const glm::mat4& transform) const;
};

// Frustum plane
struct FrustumPlane
{
    glm::vec3 normal;
    float distance;

    FrustumPlane() : normal(0.0f), distance(0.0f) {}
    FrustumPlane(const glm::vec3& normal, float distance) : normal(normal), distance(distance) {}

    // Normalize the plane
    void Normalize()
    {
        float length = glm::length(normal);
        normal /= length;
        distance /= length;
    }

    // Get signed distance from plane to point
    float GetSignedDistance(const glm::vec3& point) const
    {
        return glm::dot(normal, point) + distance;
    }
};

// View frustum for culling
class Frustum
{
public:
    enum PlaneIndex
    {
        Near = 0,
        Far,
        Left,
        Right,
        Top,
        Bottom,
        PlaneCount
    };

    Frustum() = default;

    // Extract frustum planes from view-projection matrix
    void Update(const glm::mat4& viewProjection);

    // Test if AABB is inside frustum (returns true if visible)
    bool IsAABBVisible(const AABB& aabb) const;

    // Test if sphere is inside frustum
    bool IsSphereVisible(const glm::vec3& center, float radius) const;

private:
    std::array<FrustumPlane, PlaneCount> m_Planes;
};
