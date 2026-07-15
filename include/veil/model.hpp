
#pragma once

#include <veil_export.h>

#include <vector>
#include <fstream>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "assets.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "cachemgr.hpp"
#include "logmgr.hpp"

namespace veil {

class VEIL_EXPORT Model {
    public:
        Model(const std::string& path);
        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;
        Model(Model&&) noexcept = default;
        Model& operator=(Model&&) noexcept = default;
        ~Model() = default;
        
        void render(Shader& shader) const; 

        inline const std::string& getDirectory() const { return m_directory; }
        inline std::span<const Mesh> getMeshesRead() const { return m_meshes; }
        inline std::vector<Mesh>& getMeshesWrite() { return m_meshes; }
        
    private:
        std::vector<Mesh> m_meshes;
        std::string m_directory;

        void loadModel(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
}; //class Model

}; //namespace veil