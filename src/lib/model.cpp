
#include <veil/model.hpp>

namespace veil {

Model::Model(const std::string& path) {

    VEIL_START_LOG_TIMER_FOR(path)

    m_directory = std::filesystem::path(path).parent_path().string();

    std::string cacheFile = m_directory + g_cacheDir + g_cacheFile;

    if (std::filesystem::exists(cacheFile)) 
        ModelStorage::getInstance().loadFromBIN(*this);
    else
        loadModel(path);
}

void Model::render(Shader& shader) const {

    for (const auto& mesh : m_meshes) 
        mesh.render(shader);
}

void Model::loadModel(const std::string& path) {

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path, 
        aiProcess_Triangulate | 
        aiProcess_FlipUVs | 
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ImproveCacheLocality
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        throw veil::Exception(importer.GetErrorString());

    processNode(scene->mRootNode, scene);

    ModelStorage::getInstance().saveToBIN(*this);
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

        if (aiMat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString relativePath;
            aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &relativePath);
            material.diffuse = TextureStorage::getInstance().loadTexture(m_directory + '/' + relativePath.C_Str());
        }
        if (aiMat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
            aiString relativePath;
            aiMat->GetTexture(aiTextureType_SPECULAR, 0, &relativePath);
            material.specular = TextureStorage::getInstance().loadTexture(m_directory + '/' + relativePath.C_Str());
        }
    }

    return Mesh(std::move(vertices), std::move(indices), material);
}

ModelInstance::ModelInstance(const Model& base) : m_base(base) {
    
    m_modelMatrix.mat = glm::mat4(1.0f);
    //TEMP
    m_modelMatrix.mat = glm::rotate(m_modelMatrix.mat, glm::radians(-90.0f), {1.0f, 0.0f, 0.0f});
    m_modelMatrix.mat = glm::scale(m_modelMatrix.mat, {0.2f, 0.2f, 0.2f});
}

void ModelInstance::render(Shader& shader) const {

    m_base.render(shader);
}

};