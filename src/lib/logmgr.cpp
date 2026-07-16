
#include <veil/logmgr.hpp>

namespace veil {

LogTimer::LogTimer(const std::string& procName) {

    m_procName = procName;
    m_startTimePoint = std::chrono::steady_clock::now();

    std::cout << std::format("VEIL::INFO Process '{}' started", m_procName) << std::endl;
}
LogTimer::~LogTimer() {

    const std::chrono::steady_clock::time_point endTimePoint = std::chrono::steady_clock::now();
    const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTimePoint - m_startTimePoint);

    std::cout << std::format("VEIL::INFO Process '{}' finished in {}", m_procName, elapsedTime) << std::endl;
}

Exception::Exception(const std::string& msg, std::source_location loc) 
    : std::runtime_error(msg), m_location(loc) {
    
    m_error_message = std::format("VEIL::ERROR {} in {}", msg, m_location.function_name());
}
const char* Exception::what() const noexcept {

    return m_error_message.c_str();
}
} //namespace veil