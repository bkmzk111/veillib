
#pragma once 

#include <veil_export.h>

#include <string>
#include <span>
#include <fstream>
#include <vector>
#include <initializer_list>

#include <glm/gtc/type_ptr.hpp>

#include "assets.hpp"
#include "logmgr.hpp"

namespace veil {

class VEIL_EXPORT Shader {
    public:
        explicit Shader(std::initializer_list<util::ShaderSourceStruct> sources);
        ~Shader();

        inline void useProgram() const { glUseProgram(m_shaderProgram); }

        void setUniform(int location, float x, float y, float z);
        void setUniform(int location, float x, float y);
        void setUniform(int location, glm::mat4 mat);
        void setUniform(int location, const Vector3& vec);
        void setUniform(int location, const Vector2& vec);
        void setUniform(int location, const Matrix4& mat);

        inline GLuint getID() const { return m_shaderProgram; }

    private:
        GLuint m_shaderProgram;

        void uniformVector3f(int location, float x, float y, float z);
        void uniformVector2f(int location, float x, float y);
        void uniformMatrix4f(int location, glm::mat4 mat);
}; //class Shader

}; //namespace veil