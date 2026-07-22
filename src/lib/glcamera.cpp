
#include <veil/glcamera.hpp>

namespace veil {

GLCamera::GLCamera(const Vector2& size, const Vector3& initPos, const Vector3& up, float aspectRatio, float fovyDeg) {

    m_position = initPos;
    m_up = up;

    m_projection.makeProjectionMat(fovyDeg, aspectRatio, 0.1f, 500.0f);
    m_view.makeUnitMat(1.0f);

    m_callbackData.lastx = size.data.x / 2.0f;
    m_callbackData.lasty = size.data.y / 2.0f;

    updateView();
}

void GLCamera::calculateAttitude(double xpos, double ypos) {

    if (m_callbackData.firstMovement) {

        m_callbackData.lastx = xpos;
        m_callbackData.lasty = ypos;
        m_callbackData.firstMovement = false;
    }

    double xoff = (xpos - m_callbackData.lastx) * 0.1f;
    double yoff = (m_callbackData.lasty - ypos) * 0.1f;

    m_callbackData.yaw += static_cast<float>(xoff);
    m_callbackData.pitch += static_cast<float>(yoff);
    m_callbackData.pitch = std::clamp(m_callbackData.pitch, -89.0f, 89.0f);

    m_callbackData.lastx = xpos;
    m_callbackData.lasty = ypos;

    updateView();
}

void GLCamera::updateView() {

    float x = std::cos(glm::radians(m_callbackData.yaw)) * std::cos(glm::radians(m_callbackData.pitch));
    float y = std::sin(glm::radians(m_callbackData.pitch));
    float z = std::sin(glm::radians(m_callbackData.yaw)) * std::cos(glm::radians(m_callbackData.pitch));

    Vector3 forward({x, y, z});
    forward.normalizeVec();

    m_view.makeViewMat(m_position, m_position + forward, m_up);
}

void GLCamera::setMouseCallback(Window* window, std::function<void(double, double)> mouseFunc) {

    m_mouseFunc = mouseFunc;

    glfwSetWindowUserPointer(window->getNativeHandle(), this);
    glfwSetCursorPosCallback(window->getNativeHandle(), GLCamera::mouseCallback);
}

void GLCamera::mouseCallback(GLFWwindow* window, double xpos, double ypos) {

    GLCamera* camera = static_cast<GLCamera*>(glfwGetWindowUserPointer(window));
    if (camera) 
        camera->getMouseCallback()(xpos, ypos);
}

}; //namespace veil