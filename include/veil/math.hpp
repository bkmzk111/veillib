
#pragma once 

#include <veil_export.h>

#include <concepts>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace veil {

template<typename T>
concept isVector = std::same_as<T, glm::vec3> || std::same_as<T, glm::vec2>;
template<typename T>
concept isMatrix = std::same_as<T, glm::mat4>;

template<typename T> requires isVector<T> || isMatrix<T>
struct VEIL_EXPORT MathBaseType {

    T data;

    MathBaseType() = default;
    MathBaseType(const T& v) : data(v) {}
    MathBaseType(T&& v) : data(std::move(v)) {}

    inline operator T() const { 
        return data; 
    }

    inline MathBaseType<T> operator+(const MathBaseType<T>& other) const { 
        return MathBaseType<T>(this->data + other.data); 
    }
    inline MathBaseType<T> operator-(const MathBaseType<T>& other) const {
        return MathBaseType<T>(this->data - other.data);
    }
    inline MathBaseType<T> operator*(const MathBaseType<T>& other) const {
        return MathBaseType<T>(this->data * other.data);
    }
    inline MathBaseType<T> operator/(const MathBaseType<T>& other) const requires isVector<T> {
        return MathBaseType<T>(this->data / other.data);
    }

    inline void normalizeVec() requires isVector<T> {
        data = glm::normalize(data);
    }

    inline void makeUnitMat(float k = 1.0f) requires isMatrix<T> {
        data = glm::mat4(k);
    }
    inline void makeProjectionMat(float fovyDeg, float aspectRatio, float zNear, float zFar) requires isMatrix<T> {
        data = glm::perspective(glm::radians(fovyDeg), aspectRatio, zNear, zFar);
    }
    inline void makeViewMat(const MathBaseType<glm::vec3>& eye, const MathBaseType<glm::vec3>& center, const MathBaseType<glm::vec3>& up) requires isMatrix<T> {      
        data = glm::lookAt(eye.data, center.data, up.data);
    }

    inline void translateMat(const MathBaseType<glm::vec3>& translation) requires isMatrix<T> {
        data = glm::translate(data, translation.data);
    }
    inline void rotateMat(float deg, const MathBaseType<glm::vec3>& dir) requires isMatrix<T> {
        data = glm::rotate(data, glm::radians(deg), dir.data);
    }
    inline void scaleMat(const MathBaseType<glm::vec3>& k) requires isMatrix<T> {
        data = glm::scale(data, k.data);
    }

}; //struct MathBaseType

using Vector3 = MathBaseType<glm::vec3>;
using Vector2 = MathBaseType<glm::vec2>;
using Matrix4 = MathBaseType<glm::mat4>;

}; //namespace veil