
#pragma once

#include <veil_export.h>

#include <chrono>
#include <unordered_map>
#include <iostream>
#include <format>

namespace veil {

class VEIL_EXPORT LogTimer {
    public:
        LogTimer(const std::string& procName);
        ~LogTimer();
    private:
        std::string m_procName;
        std::chrono::steady_clock::time_point m_startTimePoint;
};

#define VEIL_START_LOG_TIMER_FOR(a) LogTimer lt##__LINE__(a);

}; //namespace veil