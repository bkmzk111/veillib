
#include <veil/shader.hpp>

namespace veil {

static std::string readFile(const std::string& filename) {

    std::ifstream file(filename, std::ios::in | std::ios::binary);

    if (!file.is_open())
        throw veil::Exception(std::format("Failed to read {}", filename));
    
    return std::string{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };
}

Shader::Shader(std::initializer_list<util::ShaderSourceStruct> sources) {

    LogTimer("shader");

    if (sources.size() == 0)
        throw veil::Exception("Failed to create an empty shader");

    m_shaderProgram = glCreateProgram();

    std::vector<GLuint> shaderIDs;
    shaderIDs.reserve(sources.size());

    try {
        std::vector<std::string> sourceFiles;
        sourceFiles.reserve(sources.size());
        std::vector<GLenum> sourceTypes;
        sourceTypes.reserve(sources.size());

        for (const auto& source : sources) {
            sourceFiles.push_back(readFile(source.filePath));
            sourceTypes.push_back(source.type);
        }

        for (size_t i = 0; i < sources.size(); ++i) {

            GLuint shader = glCreateShader(sourceTypes[i]);
            shaderIDs.push_back(shader);

            const char* srcPtr = sourceFiles[i].c_str();
            glShaderSource(shader, 1, &srcPtr, nullptr);

            glCompileShader(shader);
            GLint success;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                GLchar infoLog[512];
                glGetShaderInfoLog(shader, 512, nullptr, infoLog);
                throw veil::Exception(
                    std::format("Failed to compile shader: {}", infoLog)
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
            throw veil::Exception(
                std::format("Failed to link shader program: {}", infoLog)
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