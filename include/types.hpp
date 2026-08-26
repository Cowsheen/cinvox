#pragma once

#include <cstdint>
#include <string>

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
        LogLevel level;
        std::string text;
    };

} //namespace cnx