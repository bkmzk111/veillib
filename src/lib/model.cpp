
#include <veil/model.hpp>

namespace veil {

Model::Model(const std::string& path) {

    m_directory = path.substr(0, path.find_last_of('/'));

    std::string cacheFile = m_directory + "/.cache/cache.bin";
    if (std::filesystem::exists(cacheFile)) {
        loadFromBINCache(cacheFile);
        return;
    }

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
        material.diffuse = loadMTLTextures(aiMat, aiTextureType_DIFFUSE);
        material.specular = loadMTLTextures(aiMat, aiTextureType_SPECULAR);
    }

    return Mesh(std::move(vertices), std::move(indices), material);
}

Texture Model::loadMTLTextures(aiMaterial* mat, aiTextureType type) {

    if (mat->GetTextureCount(type) == 0)
        return Texture{0, "\0"};

    aiString path;
    mat->GetTexture(type, 0, &path);

    for (const auto& texture : m_texturesLoaded)
        if (texture.path == path.C_Str())
            return texture;
    
    std::string fullPath = m_directory + '/' + path.C_Str();
    GLuint textureID = loadTextureFromFile(fullPath);

    Texture texture{textureID, path.C_Str()};
    m_texturesLoaded.push_back(texture);
    return texture;
}

Texture Model::loadMTLFromPath(const std::string& path) {

    /*for (const auto& texture : m_texturesLoaded)
        if (texture.path == path)
            return texture;*/
    
    GLuint textureID = loadTextureFromFile(path);
    Texture texture{textureID, path};
    m_texturesLoaded.push_back(texture);
    return texture;
}

void Model::saveToBINCache(const std::string& folderPath) {

    std::ofstream out(folderPath + "/cache.bin", std::ios::binary);

    unsigned int meshNum = m_meshes.size();
    out.write(reinterpret_cast<char*>(&meshNum), sizeof(meshNum));

    for (const auto& mesh : m_meshes) {

        unsigned int diffLen = mesh.getMaterial().diffuse.path.length();
        unsigned int specLen = mesh.getMaterial().specular.path.length();
        unsigned int vertCount = mesh.getVertices().size();
        unsigned int indCount = mesh.getIndices().size();

        out.write(reinterpret_cast<char*>(&diffLen), sizeof(diffLen));
        out.write(reinterpret_cast<char*>(&specLen), sizeof(specLen));
        out.write(reinterpret_cast<char*>(&vertCount), sizeof(vertCount));
        out.write(reinterpret_cast<char*>(&indCount), sizeof(indCount));

        out.write(reinterpret_cast<const char*>(mesh.getVertices().data()), vertCount*sizeof(Vertex));
        out.write(reinterpret_cast<const char*>(mesh.getIndices().data()), indCount*sizeof(unsigned int));

        if (diffLen > 0) out.write(mesh.getMaterial().diffuse.path.c_str(), diffLen);
        if (specLen > 0) out.write(mesh.getMaterial().specular.path.c_str(), specLen);
    }
}

void Model::loadFromBINCache(const std::string& filePath) {

    std::ifstream in(filePath, std::ios::binary);

    unsigned int meshNum = 0;
    in.read(reinterpret_cast<char*>(&meshNum), sizeof(meshNum));
    m_meshes.reserve(meshNum);

    for (unsigned int i = 0; i < meshNum; ++i) {

        unsigned int diffLen = 0;
        unsigned int specLen = 0;
        unsigned int vertCount = 0;
        unsigned int indCount = 0;

        in.read(reinterpret_cast<char*>(&diffLen), sizeof(diffLen));
        in.read(reinterpret_cast<char*>(&specLen), sizeof(specLen));
        in.read(reinterpret_cast<char*>(&vertCount), sizeof(vertCount));
        in.read(reinterpret_cast<char*>(&indCount), sizeof(indCount));

        std::vector<Vertex> vertices(vertCount);
        std::vector<unsigned int> indices(indCount);
        in.read(reinterpret_cast<char*>(vertices.data()), vertCount*sizeof(Vertex));
        in.read(reinterpret_cast<char*>(indices.data()), indCount*sizeof(unsigned int));

        Texture diffTexture{0, "\0"};
        if (diffLen > 0) {

            std::string diffPath(diffLen, '\0');
            in.read(diffPath.data(), diffLen);
            
            std::string filename = std::filesystem::path(diffPath).filename().string();
            diffTexture = loadMTLFromPath(m_directory + '/' + filename);
        }
        Texture specTexture{0, "\0"};
        if (specLen > 0) {

            std::string specPath(specLen, '\0');
            in.read(specPath.data(), specLen);

            std::string filename = std::filesystem::path(specPath).filename().string();
            specTexture = loadMTLFromPath(m_directory + '/' + filename);
        }
        Material material{diffTexture, specTexture};

        m_meshes.emplace_back(std::move(vertices), std::move(indices), material);
    }
}

};