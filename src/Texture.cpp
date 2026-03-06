#include "Texture.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture()
    : m_TextureID(0)
    , m_Width(0)
    , m_Height(0)
    , m_Channels(0)
    , m_IsLoaded(false)
{
    glGenTextures(1, &m_TextureID);
}

Texture::~Texture()
{
    if (m_TextureID != 0)
    {
        glDeleteTextures(1, &m_TextureID);
    }
}

bool Texture::LoadFromFile(const std::string& path, bool flipVertically)
{
    stbi_set_flip_vertically_on_load(flipVertically);
    
    unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);
    
    if (!data)
    {
        std::cerr << "Failed to load texture: " << path << std::endl;
        std::cerr << "STB Error: " << stbi_failure_reason() << std::endl;
        return false;
    }

    GenerateTexture(data, m_Width, m_Height, m_Channels);
    
    stbi_image_free(data);
    
    m_IsLoaded = true;
    std::cout << "Texture loaded: " << path << " (" << m_Width << "x" << m_Height << ", " << m_Channels << " channels)" << std::endl;
    
    return true;
}

void Texture::CreateSolidColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    unsigned char data[4] = { r, g, b, a };
    m_Width = 1;
    m_Height = 1;
    m_Channels = 4;
    
    GenerateTexture(data, 1, 1, 4);
    
    m_IsLoaded = true;
    std::cout << "Solid color texture created: (" << (int)r << ", " << (int)g << ", " << (int)b << ", " << (int)a << ")" << std::endl;
}

void Texture::Bind(unsigned int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetWrapMode(GLenum wrapS, GLenum wrapT)
{
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetFilterMode(GLenum minFilter, GLenum magFilter)
{
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::GenerateTexture(unsigned char* data, int width, int height, int channels)
{
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    GLenum format = GL_RGB;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set default texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}
