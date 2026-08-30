#pragma once

#include <condition_variable>
#include <source_location>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <format>
#include <fstream>

#include "types.hpp"

namespace cnx {

    class CinVox {
        public:
            static CinVox& instance();

            void set_file_output(std::string_view path);

            // base logging functions
            template<typename... Args>
            void log(LogLevel level,
                    const std::source_location& location,
                    std::format_string<Args...> fmt,
                    Args&&... args    
                ) {
                    if(level < m_min_log_level.load(std::memory_order_relaxed)) [[likely]] {
                        return;
                    }

                    std::string text;
                    try {
                        text = std::format(fmt, std::forward<Args>(args)...);
                    } catch (const std::exception& e) {
                        text = std::format("format error in \"{}\": {}", fmt.get(), e.what());
                    }

                    enqueue(LogMessage{ .timestamp = std::chrono::system_clock::now(),
                                        .level = level,
                                        .location = location,
                                        .text = text});
                };

            template<typename... Args>
            void log(LogLevel level,
                    std::format_string<Args...> fmt,
                    Args&&... args
                ) {
                    log(level, std::source_location::current(), fmt, std::forward<Args>(args)...);
                };

            // convinient wrappers
            template<typename... Args>
            void trace(const std::source_location& location, std::format_string<Args...> fmt, Args&&... args) {
                log(LogLevel::Trace, location, fmt, std::forward<Args>(args)...);
            }

            template<typename... Args>
            void debug(const std::source_location& location, std::format_string<Args...> fmt, Args&&... args) {
                log(LogLevel::Debug, location, fmt, std::forward<Args>(args)...);
            }

            template<typename... Args>
            void info(const std::source_location& location, std::format_string<Args...> fmt, Args&&... args) {
                log(LogLevel::Info, location, fmt, std::forward<Args>(args)...);
            }

            template<typename... Args>
            void warning(const std::source_location& location, std::format_string<Args...> fmt, Args&&... args) {
                log(LogLevel::Warning, location, fmt, std::forward<Args>(args)...);
            }

            template<typename... Args>
            void error(const std::source_location& location, std::format_string<Args...> fmt, Args&&... args) {
                log(LogLevel::Error, location, fmt, std::forward<Args>(args)...);
            }

            template<typename... Args>
            void fatal(const std::source_location& location, std::format_string<Args...> fmt, Args&&... args) {
                log(LogLevel::Fatal, location, fmt, std::forward<Args>(args)...);
            }

            template<typename... Args>
            void trace(std::format_string<Args...> fmt, Args&&... args) {
                log(LogLevel::Trace, fmt, std::forward<Args>(args)...);
            }

            template<typename... Args>
            void debug(std::format_string<Args...> fmt, Args&&... args) {
                log(LogLevel::Debug, fmt, std::forward<Args>(args)...);
            }

            template<typename... Args>
            void info(std::format_string<Args...> fmt, Args&&... args) {
                log(LogLevel::Info, fmt, std::forward<Args>(args)...);
            }

            template<typename... Args>
            void warning(std::format_string<Args...> fmt, Args&&... args) {
                log(LogLevel::Warning, fmt, std::forward<Args>(args)...);
            }

            template<typename... Args>
            void error(std::format_string<Args...> fmt, Args&&... args) {
                log(LogLevel::Error, fmt, std::forward<Args>(args)...);
            }

            template<typename... Args>
            void fatal(std::format_string<Args...> fmt, Args&&... args) {
                log(LogLevel::Fatal, fmt, std::forward<Args>(args)...);
            }

            CinVox(const CinVox&)            = delete;
            CinVox& operator=(const CinVox&) = delete;
            CinVox(CinVox&&)                 = delete;
            CinVox& operator=(CinVox&&)      = delete;

        private:
            CinVox();
            ~CinVox();

            void run(std::stop_token st);
            void enqueue(LogMessage msg);
            void write(const LogMessage& msg);
            void shutdown();

            [[nodiscard]] std::string format_line(const LogMessage& msg) const;

            // producer/consumer
            std::jthread m_worker;
            std::queue<cnx::LogMessage> m_queue;
            std::mutex m_queue_mutex;
            std::condition_variable m_cv;

            // output
            std::mutex m_file_mutex;
            std::ofstream m_file;

            // configuration
            std::atomic<LogLevel> m_min_log_level;
    };

} // namespace cnx