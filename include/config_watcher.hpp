#pragma once

#include <filesystem>
#include <string>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace cnx {

    class ConfigWatcher {
        public:
            explicit ConfigWatcher(std::string path, std::chrono::milliseconds poll_interval = std::chrono::milliseconds(1000));

            ConfigWatcher(const ConfigWatcher&) = delete;
            ConfigWatcher& operator=(const ConfigWatcher&) = delete;
            
            void shutdown();
            ~ConfigWatcher();
        private:
            void seed() noexcept;
            void run(std::stop_token st);
            bool changed() const noexcept;
            void reload();

            std::string m_path;
            std::chrono::milliseconds m_poll_interval;

            std::filesystem::file_time_type m_last_mtime;
            std::uintmax_t m_last_size;
            bool m_failed;

            std::mutex m_cv_mutex;
            std::condition_variable_any m_cv;
            std::jthread m_worker;
            std::atomic<bool> m_stopped;
    };

} // namespace cnx