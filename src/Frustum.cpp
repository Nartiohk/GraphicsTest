#include "Frustum.h"
#include <limits>

AABB AABB::Transform(const glm::mat4& transform) const
{
    // Get all 8 corners of the AABB
    glm::vec3 corners[8] = {
        glm::vec3(min.x, min.y, min.z),
        glm::vec3(max.x, min.y, min.z),
        glm::vec3(min.x, max.y, min.z),
        glm::vec3(max.x, max.y, min.z),
        glm::vec3(min.x, min.y, max.z),
        glm::vec3(max.x, min.y, max.z),
        glm::vec3(min.x, max.y, max.z),
        glm::vec3(max.x, max.y, max.z)
    };

    // Transform all corners
    glm::vec3 newMin(std::numeric_limits<float>::max());
    glm::vec3 newMax(std::numeric_limits<float>::lowest());

    for (int i = 0; i < 8; ++i)
    {
        glm::vec4 transformed = transform * glm::vec4(corners[i], 1.0f);
        glm::vec3 transformedPos = glm::vec3(transformed) / transformed.w;

        newMin = glm::min(newMin, transformedPos);
        newMax = glm::max(newMax, transformedPos);
    }

    return AABB(newMin, newMax);
}

void Frustum::Update(const glm::mat4& viewProjection)
{
    // Extract planes from view-projection matrix
    // Left plane
    m_Planes[Left].normal.x = viewProjection[0][3] + viewProjection[0][0];
    m_Planes[Left].normal.y = viewProjection[1][3] + viewProjection[1][0];
    m_Planes[Left].normal.z = viewProjection[2][3] + viewProjection[2][0];
    m_Planes[Left].distance = viewProjection[3][3] + viewProjection[3][0];
    m_Planes[Left].Normalize();

    // Right plane
    m_Planes[Right].normal.x = viewProjection[0][3] - viewProjection[0][0];
    m_Planes[Right].normal.y = viewProjection[1][3] - viewProjection[1][0];
    m_Planes[Right].normal.z = viewProjection[2][3] - viewProjection[2][0];
    m_Planes[Right].distance = viewProjection[3][3] - viewProjection[3][0];
    m_Planes[Right].Normalize();

    // Bottom plane
    m_Planes[Bottom].normal.x = viewProjection[0][3] + viewProjection[0][1];
    m_Planes[Bottom].normal.y = viewProjection[1][3] + viewProjection[1][1];
    m_Planes[Bottom].normal.z = viewProjection[2][3] + viewProjection[2][1];
    m_Planes[Bottom].distance = viewProjection[3][3] + viewProjection[3][1];
    m_Planes[Bottom].Normalize();

    // Top plane
    m_Planes[Top].normal.x = viewProjection[0][3] - viewProjection[0][1];
    m_Planes[Top].normal.y = viewProjection[1][3] - viewProjection[1][1];
    m_Planes[Top].normal.z = viewProjection[2][3] - viewProjection[2][1];
    m_Planes[Top].distance = viewProjection[3][3] - viewProjection[3][1];
    m_Planes[Top].Normalize();

    // Near plane
    m_Planes[Near].normal.x = viewProjection[0][3] + viewProjection[0][2];
    m_Planes[Near].normal.y = viewProjection[1][3] + viewProjection[1][2];
    m_Planes[Near].normal.z = viewProjection[2][3] + viewProjection[2][2];
    m_Planes[Near].distance = viewProjection[3][3] + viewProjection[3][2];
    m_Planes[Near].Normalize();

    // Far plane
    m_Planes[Far].normal.x = viewProjection[0][3] - viewProjection[0][2];
    m_Planes[Far].normal.y = viewProjection[1][3] - viewProjection[1][2];
    m_Planes[Far].normal.z = viewProjection[2][3] - viewProjection[2][2];
    m_Planes[Far].distance = viewProjection[3][3] - viewProjection[3][2];
    m_Planes[Far].Normalize();
}

bool Frustum::IsAABBVisible(const AABB& aabb) const
{
    // Test if AABB is on the positive side of all planes
    for (const auto& plane : m_Planes)
    {
        // Get the positive vertex (furthest along plane normal)
        glm::vec3 positiveVertex = aabb.min;
        if (plane.normal.x >= 0) positiveVertex.x = aabb.max.x;
        if (plane.normal.y >= 0) positiveVertex.y = aabb.max.y;
        if (plane.normal.z >= 0) positiveVertex.z = aabb.max.z;

        // If positive vertex is behind plane, AABB is outside frustum
        if (plane.GetSignedDistance(positiveVertex) < 0)
            return false;
    }

    return true;
}

bool Frustum::IsSphereVisible(const glm::vec3& center, float radius) const
{
    // Test if sphere is on the positive side of all planes
    for (const auto& plane : m_Planes)
    {
        if (plane.GetSignedDistance(center) < -radius)
            return false;
    }

    return true;
}
