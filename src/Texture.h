#pragma once
#include <glad/glad.h>
#include <string>

class Texture
{
public:
    Texture();
    ~Texture();

    // Load texture from file
    bool LoadFromFile(const std::string& path, bool flipVertically = true);
    
    // Create a solid color texture
    void CreateSolidColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

    // Bind texture to a specific texture unit
    void Bind(unsigned int unit = 0) const;
    void Unbind() const;

    unsigned int GetID() const { return m_TextureID; }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    int GetChannels() const { return m_Channels; }
    bool IsLoaded() const { return m_IsLoaded; }

    // Texture parameters
    void SetWrapMode(GLenum wrapS, GLenum wrapT);
    void SetFilterMode(GLenum minFilter, GLenum magFilter);

private:
    unsigned int m_TextureID;
    int m_Width;
    int m_Height;
    int m_Channels;
    bool m_IsLoaded;

    void GenerateTexture(unsigned char* data, int width, int height, int channels);
};
