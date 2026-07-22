
#include "veil/glrenderer.hpp"

namespace veil {

void GLRenderer::setTargets(std::initializer_list<util::RenderTarget> targets) {

    for (const auto& targ : targets) 
        m_renderData[&targ.shader].push_back(&targ.modelInst);
}

void GLRenderer::callbackUniforms() const {

    if (m_uniformData.empty())
        return;

    for (const auto& [shader, uniforms] : m_uniformData) {

        shader->useProgram();

        for (const auto& [location, setter] : uniforms)
            setter(*shader, location);
    }
}

void GLRenderer::callbackRender() const {

    if (m_renderData.empty())
        return;

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& data : m_renderData) {

        auto shader = data.first;
        auto models = data.second;

        for (const auto& model : models) {

            shader->setUniform(8, model->getModelMat());
            model->render(*shader);
        }
    }
}

};