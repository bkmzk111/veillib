
#pragma once

#include <veil_export.h>

#include <vector>

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
    private:
        std::vector<Mesh> m_meshes;
        std::string m_directory;
        std::vector<TextureCache> m_texturesLoaded;

        void loadModel(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        GLuint loadMTLTextures(aiMaterial* mat, aiTextureType type);
};

};