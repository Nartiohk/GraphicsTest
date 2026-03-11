#include "CubeShadowMap.h"
#include <iostream>

CubeShadowMap::CubeShadowMap(unsigned int resolution)
    : m_Resolution(resolution)
{
    // Generate framebuffer
    glGenFramebuffers(1, &m_FBO);

    // Generate depth cubemap
    glGenTextures(1, &m_DepthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_DepthCubemap);

    // Create 6 depth texture faces
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                     m_Resolution, m_Resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Attach depth cubemap to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_DepthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "ERROR: Cube shadow map framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

CubeShadowMap::~CubeShadowMap()
{
    glDeleteFramebuffers(1, &m_FBO);
    glDeleteTextures(1, &m_DepthCubemap);
}

void CubeShadowMap::BindForWriting(unsigned int cubeFace) const
{
    glViewport(0, 0, m_Resolution, m_Resolution);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                          GL_TEXTURE_CUBE_MAP_POSITIVE_X + cubeFace, m_DepthCubemap, 0);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void CubeShadowMap::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CubeShadowMap::BindTexture(unsigned int textureUnit) const
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_DepthCubemap);
}
