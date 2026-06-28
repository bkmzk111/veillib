
#include <lib.hpp>

namespace veil {

static void initializeGlfw() {

    static bool isInitialized = false;
    if (!isInitialized) {
        if (!glfwInit())
            throw std::runtime_error("VEIL::GLFW::CRITICAL Failed to initialize GLFW");
        isInitialized = true;
    }
}

Window::Window(const std::string& title, int width, int height) {

    initializeGlfw();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window) 
        throw std::runtime_error("VEIL::GLFW::CRITICAL Failed to create GLFW window");

    glfwMakeContextCurrent(m_window);
    
    glfwSwapInterval(1);
    glfwSetWindowUserPointer(m_window, this);

    glfwSetFramebufferSizeCallback(m_window, Window::framebufferSizeCallback);
}
Window::~Window() {
    
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}
void Window::pollEvents() const {
    glfwPollEvents();
}
void Window::swapBuffers() const {
    glfwSwapBuffers(m_window);
}

void Window::getSize(int& width, int& height) const {
    glfwGetFramebufferSize(m_window, &width, &height);
}
float Window::getAspectRatio() const {
    
    int w, h;
    getSize(w, h);
    return (h > 0) ? static_cast<float>(w) / h : 1.0f;
}
GLFWwindow* Window::getNativeHandle() const {
    return m_window;
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {

    auto* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (instance && glViewport != nullptr) 
        glViewport(0, 0, width, height);
}

}; //namespace veil