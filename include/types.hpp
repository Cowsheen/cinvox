#pragma once

#include <cstdint>
#include <source_location>
#include <string>
#include <chrono>

namespace cnx {

    enum class LogLevel: uint8_t {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Fatal,
        Off
    };

    struct LogMessage {
        std::chrono::system_clock::time_point timestamp;
        LogLevel level;
        std::source_location location;
        std::string text;
    };

} //namespace cnx