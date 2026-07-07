
#pragma once

#include <veil_export.h>

#include <vector>

#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "assets.hpp"
#include "shader.hpp"

namespace veil {

class VEIL_EXPORT Mesh {
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        ~Mesh() = default;

        void render(Shader& shader) const;

        inline const auto& getVertices() const { return m_vertices; }
        inline const auto& getIndices() const { return m_indices; }
        inline const auto& getTextures() const { return m_textures; }
    private:
        std::vector<Vertex> m_vertices;
        std::vector<unsigned int> m_indices;
        std::vector<Texture> m_textures;

        GLuint m_vao, m_vbo, m_ebo;
        
        void setup();
};

class VEIL_EXPORT Model {
    public:
        Model(const std::string& path);
        ~Model() = default;
        
        void render(Shader& shader) const; 
    private:
        std::vector<Mesh> m_meshes;
        std::string m_directory;
        std::vector<Texture> m_texturesLoaded;

        void loadModel(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture> loadMTLTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
};

};