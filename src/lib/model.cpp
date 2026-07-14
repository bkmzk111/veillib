
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

    size_t totalBytes = sizeof(unsigned int);

    for (const auto& mesh : m_meshes) {

        totalBytes += sizeof(util::BINCacheHeader);
        totalBytes += mesh.getMaterial().diffuse.path.length();
        totalBytes += mesh.getMaterial().specular.path.length();
        totalBytes += mesh.getVertices().size() * sizeof(Vertex);
        totalBytes += mesh.getIndices().size() * sizeof(unsigned int);
    }

    std::vector<char> fileBuffer(totalBytes);
    char* bufferPtr = fileBuffer.data();

    unsigned int meshNum = static_cast<unsigned int>(m_meshes.size());
    std::memcpy(bufferPtr, &meshNum, sizeof(meshNum));
    bufferPtr += sizeof(meshNum);

    util::BINCacheHeader header;

    for (const auto& mesh : m_meshes) {

        const auto& diffPath = mesh.getMaterial().diffuse.path;
        const auto& specPath = mesh.getMaterial().specular.path;

        header.diffLen = static_cast<unsigned int>(diffPath.length());
        header.specLen = static_cast<unsigned int>(specPath.length());
        header.vertCount = static_cast<unsigned int>(mesh.getVertices().size());
        header.indCount = static_cast<unsigned int>(mesh.getIndices().size());
        std::memcpy(bufferPtr, &header, sizeof(header));
        bufferPtr += sizeof(header);

        if (header.diffLen > 0) {
            std::memcpy(bufferPtr, diffPath.data(), header.diffLen);
            bufferPtr += header.diffLen;
        }
        if (header.specLen > 0) {
            std::memcpy(bufferPtr, specPath.data(), header.specLen);
            bufferPtr += header.specLen;
        }

        size_t vertBytes = header.vertCount * sizeof(Vertex);
        std::memcpy(bufferPtr, mesh.getVertices().data(), vertBytes);
        bufferPtr += vertBytes;

        size_t indBytes = header.indCount * sizeof(unsigned int);
        std::memcpy(bufferPtr, mesh.getIndices().data(), indBytes);
        bufferPtr += indBytes;
    }

    out.write(fileBuffer.data(), totalBytes);
}

void Model::loadFromBINCache(const std::string& filePath) {

    std::ifstream in(filePath, std::ios::binary | std::ios::ate);

    std::streamsize fileSize = in.tellg();
    in.seekg(0, std::ios::beg);

    std::vector<char> fileBuffer(fileSize);
    in.read(fileBuffer.data(), fileSize);
    
    const char* bufferPtr = fileBuffer.data();

    unsigned int meshNum = 0;
    std::memcpy(&meshNum, bufferPtr, sizeof(meshNum));
    bufferPtr += sizeof(meshNum);

    m_meshes.reserve(meshNum);

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    util::BINCacheHeader header;

    for (unsigned int i = 0; i < meshNum; ++i) {

        std::memcpy(&header, bufferPtr, sizeof(header));
        bufferPtr += sizeof(header);

        Texture diffTexture{0, "\0"};
        if (header.diffLen > 0) {

            std::string diffPath(bufferPtr, header.diffLen);
            bufferPtr += header.diffLen;
            diffTexture = TextureStorage::getInstance().loadTexture(diffPath);
        }
        Texture specTexture{0, "\0"};
        if (header.specLen > 0) {

            std::string specPath(bufferPtr, header.specLen);
            bufferPtr += header.specLen;
            specTexture = TextureStorage::getInstance().loadTexture(specPath);
        }
        Material material{diffTexture, specTexture};

        vertices.resize(header.vertCount);
        indices.resize(header.indCount);

        size_t vertBytes = sizeof(Vertex) * header.vertCount;
        size_t indBytes = sizeof(unsigned int) * header.indCount;

        std::memcpy(vertices.data(), bufferPtr, vertBytes);
        bufferPtr += vertBytes;
        std::memcpy(indices.data(), bufferPtr, indBytes);
        bufferPtr += indBytes;

        m_meshes.emplace_back(std::move(vertices), std::move(indices), material);
    }
}

};