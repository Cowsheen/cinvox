#pragma once
#include "config_watcher.hpp"
#include <atomic>
#include <chrono>
#include <mutex>

namespace cnx {
    class LogManager {
        public:
            static LogManager& instance();
            void init(std::string config_path, std::chrono::milliseconds poll_interval = std::chrono::milliseconds(1000));
            void shutdown();
        private:
            LogManager();

            std::mutex m_mutex;
            std::string m_config_path;
            std::unique_ptr<ConfigWatcher> m_watcher;
            std::atomic<bool> m_shutdown;
    };
} // namespace cnx