
#pragma once

#include <veil_export.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace veil {

#define VEIL_INIT_OPENGL_DRV() \
if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) { \
    throw std::runtime_error("VEIL::GLAD::CRITICAL Failed to initialize GLAD"); \
}

class VEIL_EXPORT Window {
    public:
        Window(const std::string& title, int width = 1280, int height = 720);
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;
        ~Window(); 

        bool shouldClose() const;
        void pollEvents()  const;
        void swapBuffers() const;

        void setUpdateCallback(std::function<void()> loopFunc);
        void startUpdateLoop() const;

        void getSize(int& width, int& height) const;
        float getAspectRatio() const;
        GLFWwindow* getNativeHandle() const;

    private:
        GLFWwindow* m_window;
        std::function<void()> m_loopFunc;

        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
}; //class Window

}; //namespace veil