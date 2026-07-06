
#include <veil/model.hpp>

namespace veil {

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures) {

    m_vertices = vertices;
    m_indices = indices;
    m_textures = textures;

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

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void Mesh::render(Shader& shader) {

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

};