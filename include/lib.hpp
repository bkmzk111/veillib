
#pragma once

#include <veil_export.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace veil {

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

        void getSize(int& width, int& height) const;
        float getAspectRatio() const;
        GLFWwindow* getNativeHandle() const;

    private:
        GLFWwindow* m_window;
        std::string m_title;

        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
}; //class Window

}; //namespace veil