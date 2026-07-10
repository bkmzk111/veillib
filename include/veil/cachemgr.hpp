
#pragma once

#include <veil_export.h>

#include <unordered_map>

#include "assets.hpp"

namespace veil {

class VEIL_EXPORT TextureStorage {
    public:
        TextureStorage() = default;
        TextureStorage(const TextureStorage&) = delete;
        TextureStorage& operator=(const TextureStorage&) = delete;
        TextureStorage(TextureStorage&&) = delete;
        TextureStorage& operator=(TextureStorage&&) = delete;
        ~TextureStorage(); 

        Texture loadTexture(const std::string& path);
        static void shutdown();

        static TextureStorage& getInstance();

    private:
        std::unordered_map<std::string, Texture> m_cache;

        GLuint loadTextureFromFile(const std::string& path);
};

};