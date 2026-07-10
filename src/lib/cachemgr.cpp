
#include <veil/cachemgr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace veil {

TextureStorage::~TextureStorage() {
    shutdown(); //fallback
}

Texture TextureStorage::loadTexture(const std::string& path) {

    if (path.empty() || path == "\0")
        return Texture{0, "\0"};

    auto it = m_cache.find(path);
    if (it != m_cache.end())
        return it->second;

    GLuint textureID = loadTextureFromFile(path);
    Texture texture{textureID, path};

    m_cache[path] = texture;

    return texture;
}

void TextureStorage::shutdown() {

    auto& instance = getInstance();
    for (const auto& [path, texture] : instance.m_cache) {
        if (texture.id) {
            glDeleteTextures(1, &texture.id);
        }
    }
    instance.m_cache.clear();
}

TextureStorage& TextureStorage::getInstance() {

    static TextureStorage ts;
    return ts;
}

GLuint TextureStorage::loadTextureFromFile(const std::string& path) {

    stbi_set_flip_vertically_on_load(true);

    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (!data) {

        stbi_image_free(data);
        throw std::runtime_error("VEIL::STBI::CRITICAL " + std::string("Failed to load data at ") + path);
        return 0;
    }

    GLenum internalFormat = 0;
    GLenum dataFormat = 0;

    if (channels == 1) {
        internalFormat = GL_R8;
        dataFormat = GL_RED;
    } 
    else if (channels == 3) {
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
    } 
    else if (channels == 4) {
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
    }

    GLuint textureID;
 
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
    glTextureStorage2D(textureID, 1, internalFormat, width, height);
    glTextureSubImage2D(textureID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);

    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return textureID;
}

};