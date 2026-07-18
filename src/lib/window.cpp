
#include <veil/window.hpp>

namespace veil {

Window::Window(std::string_view title, int width, int height) {

    m_loopFunc = nullptr;

    if (!glfwInit())
        throw veil::Exception("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw veil::Exception("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);

    #ifdef WIN32
    VEIL_INIT_OPENGL_DRV // WIN32 glad needs this macro to run explicitely in the .dll code
                         // or else glad function pointers will be 0x0
                         // THE CODE HAS NOT BEEN TESTED ON LINUX YET
    #endif 

    glfwSetWindowAttrib(m_window, GLFW_RESIZABLE, GLFW_FALSE);
    glfwSwapInterval(1);    
}
Window::~Window() {
    
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

void Window::setUpdateCallback(const std::function<void()>& loopFunc) {
    m_loopFunc = loopFunc;
}
void Window::startUpdateLoop() const {

    if (!m_loopFunc)
        return;
    
    while (!this->shouldClose()) {

        this->pollEvents();
        m_loopFunc();
        this->swapBuffers();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

float Window::getAspectRatio() const {
    
    int w, h;
    getSize(w, h);
    return (h > 0) ? static_cast<float>(w) / h : 1.0f;
}

}; //namespace veil