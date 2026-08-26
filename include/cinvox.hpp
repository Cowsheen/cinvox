#pragma once

#include <condition_variable>
#include <source_location>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <format>

#include "types.hpp"

namespace cnx {

    class CinVox {
        public:
            static CinVox& instance();

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

                    enqueue(LogMessage{level, text});
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
            void trace(std::format_string<Args...> fmt, Args&&... args) {
                log(LogLevel::Trace, fmt, std::forward<Args>(args)...);
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

            // producer/consumer
            std::jthread m_worker;
            std::queue<cnx::LogMessage> m_queue;
            std::mutex m_queue_mutex;
            std::condition_variable m_cv;

            // configuration
            std::atomic<LogLevel> m_min_log_level;
    };

} // namespace cnx