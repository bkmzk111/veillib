
#include <veil/model.hpp>

namespace veil {

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {

    m_vertices = std::move(vertices);
    m_indices = std::move(indices);
    m_textures = std::move(textures);

    setup();
}

void Mesh::setup() {

    glCreateVertexArrays(1, &m_vao);

    glCreateBuffers(1, &m_vbo);
    glNamedBufferStorage(m_vbo, m_vertices.size()*sizeof(Vertex), &m_vertices[0], 0);
    glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(Vertex));

    glCreateBuffers(1, &m_ebo);
    glNamedBufferStorage(m_ebo, m_indices.size()*sizeof(unsigned int), &m_indices[0], 0);
    glVertexArrayElementBuffer(m_vao, m_ebo);

    glEnableVertexArrayAttrib(m_vao, 0);
    glVertexArrayAttribBinding(m_vao, 0, 0);
    glVertexArrayAttribFormat(m_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));

    glEnableVertexArrayAttrib(m_vao, 1);
    glVertexArrayAttribBinding(m_vao, 1, 0);
    glVertexArrayAttribFormat(m_vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    
    glEnableVertexArrayAttrib(m_vao, 2);
    glVertexArrayAttribBinding(m_vao, 2, 0);
    glVertexArrayAttribFormat(m_vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texuv));
}

/*Mesh::~Mesh() {
    
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);

    for (const auto& texture : m_textures)
        glDeleteTextures(1, &texture.id);
}*/

void Mesh::render(Shader& shader) const {

    unsigned int diffuseCount = 0;
    unsigned int specularCount = 0;

    for (unsigned int i = 0; i < m_textures.size(); ++i) {

        std::string type = m_textures[i].type;
        GLuint id = m_textures[i].id;
        if (type == "texture_diffuse" && diffuseCount < 1) {
            glBindTextureUnit(0 + diffuseCount, id);
            diffuseCount++;
        }
        else if (type == "texture_specular" && specularCount < 1) {
            glBindTextureUnit(1 + specularCount, id);
            specularCount++;
        }
    }

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
}

Model::Model(const std::string& path) {

    loadModel(path);
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
    std::vector<Texture> textures;

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

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMTLTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMTLTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMTLTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName) {

    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {

        aiString path;
        mat->GetTexture(type, i, &path);

        bool skip = false;
        for (const auto& texture : m_texturesLoaded)
            if (std::strcmp(texture.path.data(), path.C_Str()) == 0) {
                textures.push_back(texture);
                skip = true;
                break;
            }

        if (!skip) {
            Texture texture;
            texture.loadFromFile(m_directory + '/' + path.C_Str());
            texture.type = typeName;
            texture.path = path.C_Str();

            textures.push_back(texture);
            m_texturesLoaded.push_back(texture);
        }
    }
    return textures;
}

};