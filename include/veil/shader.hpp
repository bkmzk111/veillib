
#pragma once 

#include <veil_export.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <span>
#include <fstream>
#include <vector>

#include <glad/glad.h>

#include "assets.hpp"

namespace veil {

class VEIL_EXPORT Shader {
    public:
        Shader(std::span<const std::string> files, std::span<const GLenum> types);
        ~Shader();

        void useProgram() const;

        void setUniform(int location, float x, float y, float z);
        void setUniform(int location, glm::mat4 mat);
        
        inline void setUniform(int location, c_vec3 auto const& vec) 
            { uniformVector3f(location, vec.vec.x, vec.vec.y, vec.vec.z); }
        inline void setUniform(int location, c_mat4 auto const& mat) 
            { uniformMatrix4f(location, mat); }

        GLuint getID() const;
    private:
        GLuint m_shaderProgram;

        void uniformVector3f(int location, float x, float y, float z);
        void uniformMatrix4f(int location, glm::mat4 mat);
}; //class Shader

}; //namespace veil