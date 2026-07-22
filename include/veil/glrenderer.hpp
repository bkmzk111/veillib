
#pragma once 

#include <veil_export.h>

#include <unordered_map>
#include <vector>
#include <initializer_list>
#include <functional>
#include <glad/glad.h>

#include "model.hpp"
#include "shader.hpp"
#include "assets.hpp"

namespace veil {

#define VEIL_INIT_OPENGL_RENDER_FLAGS \
    glEnable(GL_DEPTH_TEST); \
    glEnable(GL_CULL_FACE); 
//VEIL_INIT_OPENGL_RENDER_FLAGS

class VEIL_EXPORT GLRenderer {
    public:
        GLRenderer() = default;

        GLRenderer(const GLRenderer&) = delete;
        GLRenderer& operator=(const GLRenderer&) = delete;
        GLRenderer(GLRenderer&&) noexcept = default;
        GLRenderer& operator=(GLRenderer&&) noexcept = default;

        ~GLRenderer() = default;

        void setTargets(std::initializer_list<std::pair<const Shader&, const ModelInstance&>> targets);

        template<typename T>
        inline void uploadUniform(const Shader& shader, GLint location, T&& v) {

            if constexpr (std::is_invocable_v<std::decay_t<T>>)
                m_uniformData[&shader].push_back({
                    location, 
                    [getter = std::forward<T>(v)](const Shader& shader, GLint location) {
                        shader.setUniform(location, getter());
                    }
                });
            else 
                m_uniformData[&shader].push_back({
                    location,
                    [value = std::forward<T>(v)](const Shader& shader, GLint location) {
                        shader.setUniform(location, value);
                    }
                });
        }

        void callbackUniforms() const;
        void callbackRender() const;

        inline const auto& getRenderData() const { return m_renderData; }

    private:
        std::unordered_map<
            const Shader*, 
            std::vector<const ModelInstance*> 

        > m_renderData;

        std::unordered_map<
            const Shader*, 
            std::vector<std::pair<GLint, std::function<void(const Shader&, GLint)>>>

        > m_uniformData;
}; //class GLRenderer

}; //namespace veil