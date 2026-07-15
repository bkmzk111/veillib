
#pragma once

#include <veil_export.h>

#include <unordered_map>
#include <stdexcept>

#include "assets.hpp"

namespace veil {

constexpr std::string g_cacheFile = "/cache.bin";
constexpr std::string g_cacheDir  = "/.cache";

class Model;

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
}; //class TextureStorage

class VEIL_EXPORT ModelStorage {
    public:
        ModelStorage() = default;
        ModelStorage(const ModelStorage&) = delete;
        ModelStorage& operator=(const ModelStorage&) = delete;
        ModelStorage(ModelStorage&&) = delete;
        ModelStorage& operator=(ModelStorage&&) = delete;
        ~ModelStorage() = default;

        static ModelStorage& getInstance();

        void saveToBIN(const Model& model);
        void loadFromBIN(Model& model);
};

}; //namespace veil