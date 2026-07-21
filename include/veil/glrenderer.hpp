
#pragma once 

#include <veil_export.h>

#include <unordered_map>
#include <vector>
#include <initializer_list>

#include "model.hpp"
#include "shader.hpp"
#include "assets.hpp"

#include <glad/glad.h>

namespace veil {

#define VEIL_INIT_OPENGL_RENDER_FLAGS \
    glEnable(GL_DEPTH_TEST); \
    glEnable(GL_CULL_FACE); 
//VEIL_INIT_RENDER_FLAGS

class VEIL_EXPORT GLRenderer {
    public:
        GLRenderer() = default;
        inline explicit GLRenderer(std::initializer_list<util::RenderTarget> targets) { addTargets(targets); };

        GLRenderer(const GLRenderer&) = delete;
        GLRenderer& operator=(const GLRenderer&) = delete;
        GLRenderer(GLRenderer&&) noexcept = default;
        GLRenderer& operator=(GLRenderer&&) noexcept = default;

        ~GLRenderer() = default;

        void addTargets(std::initializer_list<util::RenderTarget> targets);

        void callbackTick();

    private:
        std::unordered_map<const ModelInstance&, std::vector<const Shader&>> m_renderData;

}; //class GLRenderer

}; //namespace veil