
#include <veil/glcamera.hpp>

namespace veil {

GLCamera::GLCamera(const Vector2& size, const Vector3& initPos, const Vector3& up, float aspectRatio, float fovyDeg) {

    m_position = initPos;
    m_up = up;

    m_projection.makeProjectionMat(fovyDeg, aspectRatio, 0.1f, 500.0f);
    m_view.makeUnitMat(1.0f);

    lastx = size.data.x / 2.0f;
    lasty = size.data.y / 2.0f;

    updateView();
}

void GLCamera::calculateAttitude(double xpos, double ypos) {

    if (firstMovement) {

        lastx = xpos;
        lasty = ypos;
        firstMovement = false;
    }

    double xoff = (xpos - lastx) * 0.1f;
    double yoff = (lasty - ypos) * 0.1f;

    yaw += static_cast<float>(xoff);
    pitch += static_cast<float>(yoff);
    pitch = std::clamp(pitch, -89.0f, 89.0f);

    lastx = xpos;
    lasty = ypos;

    updateView();
}

void GLCamera::updateView() {

    float x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    float y = std::sin(glm::radians(pitch));
    float z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));

    Vector3 forward({x, y, z});
    forward.normalizeVec();

    m_view.makeViewMat(m_position, m_position + forward, m_up);
}

}; //namespace veil