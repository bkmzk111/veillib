
#pragma once

#include <veil_export.h>

#include <concepts>
#include <string>

#include <glm/glm.hpp>
#include <glad/glad.h>

namespace veil {

struct VEIL_EXPORT Vector3 {
    glm::vec3 vec = {0.0f, 0.0f, 0.0f};
    operator glm::vec3() const { return vec; }
};
struct VEIL_EXPORT Vector2 {
    glm::vec2 vec = {0.0f, 0.0f};
    operator glm::vec2() const { return vec; }
};
struct VEIL_EXPORT Matrix4 {
    glm::mat4 mat = glm::mat4(1.0f);
    operator glm::mat4() const { return mat; }
};

struct VEIL_EXPORT Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 texuv;
};
struct VEIL_EXPORT Texture {
    GLuint id = 0u;
    std::string path = "\0";
};
struct VEIL_EXPORT Material {
    Texture diffuse;
    Texture specular;
};

template<typename T>
concept c_vec3 = requires(T v) {
    {v.vec.x} -> std::convertible_to<float>;
    {v.vec.y} -> std::convertible_to<float>;
    {v.vec.z} -> std::convertible_to<float>;
};
template<typename T>
concept c_vec2 = requires(T v) {
    {v.vec.x} -> std::convertible_to<float>;
    {v.vec.y} -> std::convertible_to<float>;
};
template<typename T>
concept c_mat4 = requires(T m) {
    {m.mat} -> std::convertible_to<glm::mat4>;
};

namespace util {

    struct BINCacheHeader {

        unsigned int diffLen;
        unsigned int specLen;
        unsigned int vertCount;
        unsigned int indCount;
    };
};

}; //namespace veil