
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
        Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures);
        ~Mesh();

        void render(Shader& shader);

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

/*class VEIL_EXPORT Model {
    public:
        Model(const std::string& path);
        
        void render(Shader& shader); 
    private:
        std::vector<Mesh> meshes;
        std::string directory;

        void loadModel(const std::string& path);
};*/

};