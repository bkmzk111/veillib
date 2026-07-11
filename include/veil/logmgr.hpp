
#pragma once

#include <veil_export.h>

#include <chrono>
#include <unordered_map>
#include <iostream>>

namespace veil {

class VEIL_EXPORT LogTimer {
    public:
        LogTimer() = default;
        LogTimer(const LogTimer&) = delete;
        LogTimer& operator=(const LogTimer&) = delete;
        LogTimer(LogTimer&&) = delete;
        LogTimer& operator=(LogTimer&&) = delete;
        ~LogTimer() = default; 

        void startFor(const std::string& procName);
        long long elapsedOf(const std::string& procName);
        void endFor(const std::string& procName);

        static LogTimer& getInstance();

    private:
        std::unordered_map<std::string, std::chrono::steady_clock::time_point> m_timeStorage; 
};

}; //namespace veil