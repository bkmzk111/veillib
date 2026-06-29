
#pragma once 

#include <veil_export.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <span>
#include <fstream>
#include <ranges>
#include <vector>
#include <array>

#include <glad/glad.h>

namespace veil {

class VEIL_EXPORT Shader {
    public:
        Shader(std::span<const std::string> files, std::span<const GLenum> types);
        ~Shader();

        void useProgram() const;

        GLuint getID() const;
    private:
        GLuint m_shaderProgram;
}; //class Shader

}; //namespace veil