
#pragma once

#include <veil_export.h>

namespace veil {

class VEIL_EXPORT CacheAssistant {
    public:
        CacheAssistant();
        CacheAssistant(const CacheAssistant&) = delete;
        CacheAssistant& operator=(const CacheAssistant&) = delete;
        ~CacheAssistant();
};

};