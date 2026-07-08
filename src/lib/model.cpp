
#include <veil/model.hpp>

namespace veil {

Model::Model(const std::string& path) {
    loadModel(path);
}

Model::~Model() {
    for (const auto& texture : m_texturesLoaded) 
        if (texture.id) 
            glDeleteTextures(1, &texture.id);
}

void Model::render(Shader& shader) const {

    for (const auto& mesh : m_meshes) 
        mesh.render(shader);
}

void Model::loadModel(const std::string& path) {

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw std::runtime_error("VEIL::ASSIMP::CRITICAL " + std::string(importer.GetErrorString()));
        return;
    }

    m_directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {

    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;

    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumFaces * 3);

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {

        Vertex vertex;
        Vector3 vector;

        vector.vec.x = mesh->mVertices[i].x;
        vector.vec.y = mesh->mVertices[i].y;
        vector.vec.z = mesh->mVertices[i].z;
        vertex.position = vector;

        vector.vec.x = mesh->mNormals[i].x;
        vector.vec.y = mesh->mNormals[i].y;
        vector.vec.z = mesh->mNormals[i].z;
        vertex.normal = vector;

        if (mesh->mTextureCoords[0]) {

            Vector2 uv;
            uv.vec.x = mesh->mTextureCoords[0][i].x;
            uv.vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texuv = uv;
        }
        else 
            vertex.texuv = Vector2({0.0f, 0.0f});

        vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {

        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) 
            indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0) {

        aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
        material.diffuse = loadMTLTextures(aiMat, aiTextureType_DIFFUSE);
        material.specular = loadMTLTextures(aiMat, aiTextureType_SPECULAR);
    }

    return Mesh(std::move(vertices), std::move(indices), material);
}

GLuint Model::loadMTLTextures(aiMaterial* mat, aiTextureType type) {

    if (mat->GetTextureCount(type) == 0)
        return 0;

    aiString path;
    mat->GetTexture(type, 0, &path);

    for (const auto& texture : m_texturesLoaded)
        if (texture.path == path.C_Str())
            return texture.id;
    
    std::string fullPath = m_directory + '/' + path.C_Str();
    GLuint texture = loadTextureFromFile(fullPath);

    m_texturesLoaded.push_back(TextureCache{texture, path.C_Str()});
    return texture;
}

};