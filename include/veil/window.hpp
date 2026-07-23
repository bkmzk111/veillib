
#pragma once

#include <veil_export.h>

#include <string>
#include <functional>
#include <string_view>
#include <chrono>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "logmgr.hpp"

namespace veil {

#define VEIL_INIT_OPENGL_DRV \
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) \
        throw veil::Exception("Failed to initialize GLAD"); 
//VEIL_INIT_OPENGL_DRV

class VEIL_EXPORT Window {
    public:
        Window(std::string_view title, int width = 1280, int height = 720);
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;
        ~Window(); 

        inline bool shouldClose() const { return glfwWindowShouldClose(m_window); }
        inline void pollEvents()  const { glfwPollEvents(); }
        inline void waitEvents()  const { glfwWaitEvents(); }
        inline void swapBuffers() const { glfwSwapBuffers(m_window); }

        void setUpdateCallback(const std::function<void()>& loopFunc);
        void setMouseCallback(std::function<void(double, double)> mouseFunc);

        void startUpdateLoop();

        inline void getSize(int& width, int& height) const { glfwGetFramebufferSize(m_window, &width, &height); };
        float getAspectRatio() const;
        inline GLFWwindow* getNativeHandle() const { return m_window; };

    private:
        GLFWwindow* m_window;
        std::function<void()> m_loopFunc;
        std::function<void(double, double)> m_mouseFunc;

        static void mouseCallback(GLFWwindow* window, double xpos, double ypos);

}; //class Window

}; //namespace veil