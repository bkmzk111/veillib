
#pragma once

#include <veil_export.h>

#include <concepts>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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