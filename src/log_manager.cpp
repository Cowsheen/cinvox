#include "log_manager.hpp"
#include "cinvox_config.hpp"
#include "cinvox.hpp"

namespace cnx {
    LogManager::LogManager(): m_config_path("")
                            , m_shutdown(false) {
        std::atexit(+[](){ instance().shutdown(); });
    }
    
    LogManager& LogManager::instance() {
        static auto* mgr = new LogManager;
        return *mgr;
    }

    void LogManager::init(std::string config_path, std::chrono::milliseconds poll_interval) {
        std::lock_guard lock(m_mutex);
        if(m_shutdown.load(std::memory_order_relaxed)) {
            return;
        }

        const bool new_path = (m_config_path != config_path);
        if(new_path) {
            configure_from_json(config_path);
            m_config_path = std::move(config_path);
            m_watcher.reset();
        }

        if(!m_watcher) {
            m_watcher = std::make_unique<ConfigWatcher>(m_config_path, poll_interval);
        }
    }

    void LogManager::shutdown() {
        if(m_shutdown.exchange(true)) {
            return;
        }

        std::unique_ptr<ConfigWatcher> watcher;
        {
            std::lock_guard lock(m_mutex);
            watcher = std::move(m_watcher);
        }

        watcher.reset();
        CinVox::shutdown_all();
    }
} // namespace cnx