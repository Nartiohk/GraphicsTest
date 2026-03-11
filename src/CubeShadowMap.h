#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

class CubeShadowMap
{
public:
    CubeShadowMap(unsigned int resolution = 1024);
    ~CubeShadowMap();

    void BindForWriting(unsigned int cubeFace) const;
    void Unbind() const;
    void BindTexture(unsigned int textureUnit) const;

    unsigned int GetDepthCubemap() const { return m_DepthCubemap; }
    unsigned int GetFBO() const { return m_FBO; }
    unsigned int GetResolution() const { return m_Resolution; }

private:
    unsigned int m_FBO;
    unsigned int m_DepthCubemap;
    unsigned int m_Resolution;
};
