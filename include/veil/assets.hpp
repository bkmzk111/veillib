
#pragma once

#include <veil_export.h>

#include <concepts>
#include <string>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace veil {

struct VEIL_EXPORT Vector3 {
    glm::vec3 vec;
    operator glm::vec3() const { return vec; }
};
struct VEIL_EXPORT Vector2 {
    glm::vec2 vec;
    operator glm::vec2() const { return vec; }
};
struct VEIL_EXPORT Matrix4 {
    glm::mat4 mat;
    operator glm::mat4() const { return mat; }
};

struct VEIL_EXPORT Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 texuv;
};
struct VEIL_EXPORT Texture {
    GLuint id;
    std::string type;
    std::string path;
    void loadFromFile(const std::string& path);
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

}; //namespace veil