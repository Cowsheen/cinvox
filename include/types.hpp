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

    struct LogConfig {
        std::string name;
        LogLevel level = LogLevel::Trace;
        bool console = true;
        std::optional<std::string> file;
    };

    [[nodiscard]] static constexpr std::string_view level_name(LogLevel level) {
        switch (level) {
            case LogLevel::Trace:   return "TRACE";
            case LogLevel::Debug:   return "DEBUG";
            case LogLevel::Info:    return "INFO";
            case LogLevel::Warning: return "WARNING";
            case LogLevel::Error:   return "ERROR";
            case LogLevel::Fatal:   return "FATAL";
            case LogLevel::Off:     return "";
            default:                return "";
        }
    }

    [[nodiscard]] static constexpr  std::string_view color(LogLevel level) {
        switch (level) {
            case LogLevel::Trace:   return "\033[90m";
            case LogLevel::Debug:   return "\033[36m";
            case LogLevel::Info:    return "\033[32m";
            case LogLevel::Warning: return "\033[33m";
            case LogLevel::Error:   return "\033[31m";
            case LogLevel::Fatal:   return "\033[1;31m";
            case LogLevel::Off:     return "";
            default:                return "";
        }
    }

} //namespace cnx