
#include <veil/shader.hpp>

namespace veil {

static std::string readFile(const std::string& filename) {

    std::ifstream file(filename, std::ios::in | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("VEIL::OPENGL::SHADER::CRITICAL Failed to read file: " + filename);
    
    return std::string{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };
}

Shader::Shader(std::span<const std::string> files, std::span<const GLenum> types) {

    if (files.empty() || types.empty())
        throw std::runtime_error("VEIL::OPENGL::SHADER::CRITICAL Failed to create an empty shader");
    if (files.size() != types.size())
        throw std::runtime_error("VEIL::OPENGL::SHADER::CRITICAL Failed to create a shader with no type/file");

    m_shaderProgram = glCreateProgram();

    std::vector<GLuint> shaderIDs;
    shaderIDs.reserve(files.size());

    try {
        std::vector<std::string> sources;
        sources.reserve(files.size());
        for (const auto& file : files)
            sources.push_back(readFile(file));

        for (size_t i = 0; i < files.size(); ++i) {

            GLuint shader = glCreateShader(types[i]);
            shaderIDs.push_back(shader);

            const char* srcPtr = sources[i].c_str();
            glShaderSource(shader, 1, &srcPtr, nullptr);

            glCompileShader(shader);
            GLint success;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                GLchar infoLog[512];
                glGetShaderInfoLog(shader, 512, nullptr, infoLog);
                throw std::runtime_error(
                    "VEIL::OPENGL::SHADER::CRITICAL Failed to compile shader: " + std::string(infoLog)
                );
            }

            glAttachShader(m_shaderProgram, shader);
        }

        glLinkProgram(m_shaderProgram);
        GLint success;
        glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetProgramInfoLog(m_shaderProgram, 512, nullptr, infoLog);
            throw std::runtime_error(
                "VEIL::OPENGL::SHADER::CRITICAL Failed to link program: " + std::string(infoLog)
            );
        }
    }
    catch(...) {
        for (const auto& shader : shaderIDs)  {

            glDetachShader(m_shaderProgram, shader);
            glDeleteShader(shader);
        }
        glDeleteProgram(m_shaderProgram);
        throw;
    }

    for (const auto& shader : shaderIDs) {

        glDetachShader(m_shaderProgram, shader);
        glDeleteShader(shader);
    }
}
Shader::~Shader() {
    glDeleteProgram(m_shaderProgram);
}

void Shader::setUniform(int location, float x, float y, float z) {
    uniformVector3f(location, x, y, z);
}
void Shader::setUniform(int location, float x, float y) {
    uniformVector2f(location, x, y);
}
void Shader::setUniform(int location, glm::mat4 mat) {
    uniformMatrix4f(location, mat);
}

void Shader::uniformVector3f(int location, float x, float y, float z) {
    glProgramUniform3f(m_shaderProgram, location, x, y, z);
}
void Shader::uniformVector2f(int location, float x, float y) {
    glProgramUniform2f(m_shaderProgram, location, x, y);
}
void Shader::uniformMatrix4f(int location, glm::mat4 mat) {
    glProgramUniformMatrix4fv(m_shaderProgram, location, 1, GL_FALSE, glm::value_ptr(mat));
}

} //namespace veil