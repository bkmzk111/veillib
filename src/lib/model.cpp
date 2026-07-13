
#include <veil/model.hpp>

namespace veil {

Model::Model(const std::string& path) {

    VEIL_START_LOG_TIMER_FOR(path)

    m_directory = std::filesystem::path(path).parent_path().string();

    std::string cacheFile = m_directory + "/.cache/cache.bin";

    if (std::filesystem::exists(cacheFile)) 
        loadFromBINCache(cacheFile);
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
        throw std::runtime_error("VEIL::ASSIMP::CRITICAL " + std::string(importer.GetErrorString()));

    processNode(scene->mRootNode, scene);

    std::string cacheDir = m_directory + "/.cache";
    std::filesystem::create_directories(cacheDir);
    saveToBINCache(cacheDir);
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

void Model::saveToBINCache(const std::string& folderPath) {

    std::ofstream out(folderPath + "/cache.bin", std::ios::binary);

    unsigned int meshNum = m_meshes.size();
    out.write(reinterpret_cast<char*>(&meshNum), sizeof(meshNum));

    util::BINCacheHeader header;

    for (const auto& mesh : m_meshes) {

        header.diffLen = mesh.getMaterial().diffuse.path.length();
        header.specLen = mesh.getMaterial().specular.path.length();
        header.vertCount = mesh.getVertices().size();
        header.indCount = mesh.getIndices().size();

        out.write(reinterpret_cast<char*>(&header), sizeof(header));

        if (header.diffLen > 0) out.write(mesh.getMaterial().diffuse.path.c_str(), header.diffLen);
        if (header.specLen > 0) out.write(mesh.getMaterial().specular.path.c_str(), header.specLen);

        out.write(reinterpret_cast<const char*>(mesh.getVertices().data()), header.vertCount*sizeof(Vertex));
        out.write(reinterpret_cast<const char*>(mesh.getIndices().data()), header.indCount*sizeof(unsigned int));
    }
}

void Model::loadFromBINCache(const std::string& filePath) {

    std::ifstream in(filePath, std::ios::binary);

    unsigned int meshNum = 0;
    in.read(reinterpret_cast<char*>(&meshNum), sizeof(meshNum));
    m_meshes.reserve(meshNum);

    util::BINCacheHeader header;

    for (unsigned int i = 0; i < meshNum; ++i) {

        in.read(reinterpret_cast<char*>(&header), sizeof(header));

        Texture diffTexture{0, "\0"};
        if (header.diffLen > 0) {

            std::string diffPath(header.diffLen, '\0');
            in.read(diffPath.data(), header.diffLen);
            diffTexture = TextureStorage::getInstance().loadTexture(diffPath);
        }
        Texture specTexture{0, "\0"};
        if (header.specLen > 0) {

            std::string specPath(header.specLen, '\0');
            in.read(specPath.data(), header.specLen);
            specTexture = TextureStorage::getInstance().loadTexture(specPath);
        }
        Material material{diffTexture, specTexture};

        std::vector<Vertex> vertices(header.vertCount);
        std::vector<unsigned int> indices(header.indCount);
        in.read(reinterpret_cast<char*>(vertices.data()), header.vertCount*sizeof(Vertex));
        in.read(reinterpret_cast<char*>(indices.data()), header.indCount*sizeof(unsigned int));

        m_meshes.emplace_back(std::move(vertices), std::move(indices), material);
    }
}

};