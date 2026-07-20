
#pragma once

#include <veil_export.h>

#include <chrono>
#include <iostream>
#include <format>
#include <source_location>
#include <stdexcept>

namespace veil {

class VEIL_EXPORT LogTimer {
    public:
        LogTimer(std::string_view name, std::source_location loc = std::source_location::current());
        ~LogTimer();
    private:
        std::string m_name;
        std::source_location m_location;
        std::chrono::steady_clock::time_point m_startTimePoint;
}; //class LogTimer

class VEIL_EXPORT Exception : public std::runtime_error {
    public:
        Exception(std::string_view, std::source_location loc = std::source_location::current());

        const char* what() const noexcept override;
    private:
        std::string m_error_message;
        std::source_location m_location;
}; //class Exception

}; //namespace veil