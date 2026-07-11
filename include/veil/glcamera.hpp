
#pragma once 

#include <veil_export.h>

#include <functional>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include "assets.hpp"
#include "window.hpp"

namespace veil {

class VEIL_EXPORT GLCamera {
    public:
        GLCamera(const Vector2& size, const Vector3& initPos, const Vector3& up, float aspectRatio, float fovyDegrees);
        ~GLCamera() = default;

        void calculateAttitude(double xpos, double ypos);
        void updateView();

        void setMouseCallback(Window* window, std::function<void(double, double)> mouseFunc);

        inline const Vector3& getPos() const { return m_position; }
        inline const Vector3& getUp() const { return m_up; }
        inline const Matrix4& getProjection() const { return m_projection; }
        inline const Matrix4& getView() const { return m_view; }
        inline const float& getYaw() const { return m_callbackData.yaw; }
        inline const float& getPitch() const { return m_callbackData.pitch; }

        inline const auto& getMouseCallback() const { return m_mouseFunc; }

    private:
        Vector3 m_position;
        Vector3 m_up;
        Matrix4 m_projection;
        Matrix4 m_view;

        struct callbackData {

            float yaw = -90.0f;
            float pitch = 0.0f;
        
            double lastx;
            double lasty;

            bool firstMovement = true;
        } m_callbackData;

        std::function<void(double, double)> m_mouseFunc;

        static void mouseCallback(GLFWwindow* window, double xpos, double ypos);

}; //class GLCamera

}; //namespace veil