
#include <veil/assets.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace veil {

void Texture::loadFromFile(const std::string& path) {

    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (!data) {

        stbi_image_free(data);
        throw std::runtime_error("VEIL::STBI::CRITICAL " + std::string("Failed to load data at ") + path);
        return;
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

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
    glCreateTextures(GL_TEXTURE_2D, 1, &id);
    glTextureStorage2D(id, 1, internalFormat, width, height);
    glTextureSubImage2D(id, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);

    glTextureParameteri(this->id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(this->id, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(this->id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(this->id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
}

};