#include "config_watcher.hpp"
#include "cinvox_config.hpp"
#include "cinvox.hpp"

namespace cnx {
    ConfigWatcher::ConfigWatcher(std::string path, std::chrono::milliseconds  poll_interval) 
    : m_path(path)
    , m_poll_interval(poll_interval)
    , m_last_mtime()
    , m_last_size(0)
    , m_failed(false)
    , m_stopped(false) {
        seed();
        m_worker = std::jthread([this](std::stop_token st) { run(st); });
    }
   
    void ConfigWatcher::shutdown() {
        if(m_stopped.exchange(true)) return;

        m_worker.request_stop();
        m_cv.notify_all();
    }

    ConfigWatcher::~ConfigWatcher() {
        shutdown();
    }

    void ConfigWatcher::seed() noexcept {
        try {
            m_last_mtime = std::filesystem::last_write_time(m_path);
            m_last_size = std::filesystem::file_size(m_path);
        } catch(...) {

        }
    }

    void ConfigWatcher::run(std::stop_token st) {
        while(!st.stop_requested()) {
            {
                std::unique_lock lock(m_cv_mutex);
                m_cv.wait_for(lock, m_poll_interval);
            }

            if(st.stop_requested()) 
                break;

            if(changed())
                reload();
        }
    }

    bool ConfigWatcher::changed() const noexcept {
        try {
            return  std::filesystem::last_write_time(m_path) != m_last_mtime ||
                    std::filesystem::file_size(m_path) != m_last_size;
        } catch (...) {
            return true;
        }
    }
    
    void ConfigWatcher::reload() {
        try {
            auto configs = load_config(m_path);
            apply_config(configs);

            try {
                m_last_mtime = std::filesystem::last_write_time(m_path);
                m_last_size = std::filesystem::file_size(m_path);
            } catch (...) {
            
            }

            if(m_failed) {
                m_failed = true;
                CinVox::get("default").info("config '{}' reloaded successfully ({} loggers)", m_path, configs.size());
            } else {
                CinVox::get("default").info("config '{}' reloaded ({} loggers)", m_path, configs.size());
            }

        } catch (const std::exception& e) {
            if(!m_failed) {
                m_failed = true;
                std::fprintf(stderr,
                             "[config] failed to reload '%s': %s — keeping previous configuration\n",
                             m_path.c_str(), e.what());
            }
        }
    }
} // namespace cnx