
#pragma once

#include <veil_export.h>

#include <chrono>
#include <unordered_map>
#include <iostream>
#include <format>
#include <source_location>
#include <stdexcept>

namespace veil {

class VEIL_EXPORT LogTimer {
    public:
        LogTimer(const std::string& procName);
        ~LogTimer();
    private:
        std::string m_procName;
        std::chrono::steady_clock::time_point m_startTimePoint;
};

class VEIL_EXPORT Exception : public std::runtime_error {
    public:
        Exception(const std::string& msg, std::source_location loc = std::source_location::current());

        const char* what() const noexcept override;
    private:
        std::string m_error_message;
        std::source_location m_location;
};

#define VEIL_START_LOG_TIMER_FOR(a) LogTimer lt##__LINE__(a);

}; //namespace veil