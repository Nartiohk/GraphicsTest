#pragma once
#include <glad/glad.h>

class ShadowMap
{
public:
    ShadowMap(unsigned int width = 2048, unsigned int height = 2048);
    ~ShadowMap();

    void Bind() const;
    void Unbind() const;
    void BindTexture(unsigned int textureUnit) const;

    unsigned int GetDepthMap() const { return m_DepthMap; }
    unsigned int GetFBO() const { return m_FBO; }

private:
    unsigned int m_FBO;
    unsigned int m_DepthMap;
    unsigned int m_Width;
    unsigned int m_Height;
};
