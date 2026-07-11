
#include <veil/logmgr.hpp>

namespace veil {

void LogTimer::startFor(const std::string& procName) {

    m_timeStorage[procName] = std::chrono::steady_clock::now();
}
long long LogTimer::elapsedOf(const std::string& procName) {

    auto it = m_timeStorage.find(procName);
    if (it == m_timeStorage.end())
        return -1; 

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - it->second
    );

    return elapsed.count();
}
void LogTimer::endFor(const std::string& procName) {

    m_timeStorage.erase(procName);
}

LogTimer& LogTimer::getInstance() {

    static LogTimer lt;
    return lt;
}

} //namespace veil