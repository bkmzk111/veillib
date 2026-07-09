
#pragma once

#include <veil_export.h>

#include <vector>
#include <fstream>
#include <filesystem>

#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "assets.hpp"
#include "shader.hpp"
#include "mesh.hpp"

namespace veil {

class VEIL_EXPORT Model {
    public:
        Model(const std::string& path);
        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;
        Model(Model&&) noexcept = default;
        Model& operator=(Model&&) noexcept = default;
        ~Model();
        
        void render(Shader& shader) const; 

        inline const std::string& getDirectory() { return m_directory; }
    private:
        std::vector<Mesh> m_meshes;
        std::string m_directory;
        std::vector<Texture> m_texturesLoaded;

        void loadModel(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        Texture loadMTLTextures(aiMaterial* mat, aiTextureType type);
        Texture loadMTLFromPath(const std::string& path);

        void saveToBINCache(const std::string& folderPath);
        void loadFromBINCache(const std::string& filePath);
};

};