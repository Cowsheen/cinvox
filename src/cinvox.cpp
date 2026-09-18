#include "cinvox.hpp"
#include <chrono>
#include <filesystem>

namespace cnx {
    CinVox& CinVox::get(std::string_view name){
        auto& reg = Registry::registry();
        std::lock_guard lock(reg.mutex);

        if(auto it = reg.map.find(std::string{name}); it != reg.map.end()) {
            return *it->second;
        }

        if(!reg.accepting) {
            return sink();
        }
        auto logger = std::make_unique<CinVox>(std::string{name});
        CinVox* ptr = logger.get();
        reg.map.emplace(std::string{name}, std::move(logger));
        return *ptr;
    }

    void CinVox::shutdown_all() {
        std::vector<CinVox*> loggers;
        {
            auto& reg = Registry::registry();
            std::lock_guard lock(reg.mutex);
            reg.accepting = false;
            loggers.reserve(reg.map.size());
            for(auto& [name, l] : reg.map) {
                loggers.push_back(l.get());
            }
        }

        for(CinVox* logger : loggers) {
            logger->shutdown();
        }
    }

    CinVox::CinVox(std::string name, bool start_worker)
        : m_worker(start_worker ? std::jthread([this](std::stop_token st){ run(st); }) : std::jthread({}))
        , m_min_log_level(LogLevel::Trace)
        , m_console_output(true)
        , m_accepting(true)
        , m_shutdown_done(false)
        , m_name(std::move(name))
    {}

    CinVox::~CinVox() {
        shutdown();
    }

    CinVox& CinVox::set_file_output(std::string_view path) {
        std::lock_guard lock(m_file_mutex);
        if(m_file_path == path) {
            return *this;
        }

        m_file.close();

        auto dir = std::filesystem::path(path).parent_path();
        if(!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        m_file.open(std::string(path), std::ios::out | std::ios::app);
        if(m_file.is_open()) {
            m_file_path = std::string(path);
        }
        return *this;
    }

    CinVox& CinVox::set_min_log_level(LogLevel level) {
        m_min_log_level.store(level, std::memory_order_relaxed);
        return *this;
    }

    CinVox& CinVox::set_console_output(bool enabled) {
        m_console_output.store(enabled, std::memory_order_relaxed);
        return *this;
    }

    const std::string& CinVox::name() const {
        return m_name;
    }

    std::vector<std::string> CinVox::names() {
        auto& reg = Registry::registry();
        std::lock_guard lock(reg.mutex);
        std::vector<std::string> out;
        out.reserve(reg.map.size());
        for(const auto& [name, _] : reg.map) {
            out.push_back(name);
        }
        return out;
    }

    // Shared no-op logger returned for unknown names after shutdown
    CinVox& CinVox::sink() {
        static auto* s = new CinVox("<sink>", false);   // leak: safe during teardown
        return *s;
    }

    void CinVox::run(std::stop_token st) {
        while(true) {
            std::queue<LogMessage> batch;

            {
                std::unique_lock lock(m_queue_mutex);
                m_cv.wait(lock, [&](){ return !m_queue.empty() || st.stop_requested(); });

                if(m_queue.empty() && st.stop_requested()) {
                    break;
                }

                std::swap(batch, m_queue);
            }

            while (!batch.empty()) {
                write(batch.front());
                batch.pop();
            }
        }
    }

    void CinVox::enqueue(LogMessage msg) {
        {
            std::lock_guard lock(m_queue_mutex);

            if(!m_accepting) return;

            m_queue.push(std::move(msg));
        }
        m_cv.notify_one();
    }

    void CinVox::write(const LogMessage& msg) {
        std::string line = format_line(msg);

        if(m_console_output.load(std::memory_order::relaxed)) {
            const std::string colored = std::string(color(msg.level)) + line + "\033[0m";
            std::fwrite(colored.data(), 1, colored.size(), stdout);
            std::fflush(stdout);
        }

        if(m_file.is_open()) {
            m_file.write(line.data(), static_cast<std::streamsize>(line.size()));
            m_file.flush();
        }
    }

    void CinVox::shutdown() {
        if(m_shutdown_done.exchange(true)) {
            return;
        }

        {
            std::lock_guard lock(m_queue_mutex);
            m_accepting = false;
        }
        
        if(m_worker.joinable()) {
            m_worker.request_stop();
            m_cv.notify_all();
            m_worker.join();
        }
    }

    std::string CinVox::format_line(const LogMessage& msg) const {
        const auto tp =  msg.timestamp;
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) % 1000;

        return std::format("[{:%Y-%m-%d %H:%M:%S}.{:03}] [{}] [{}] [{}:{}] {}\n",
                            tp, ms.count(),
                            m_name,
                            level_name(msg.level),
                            basename(msg.location.file_name()),
                            msg.location.line(),
                            msg.text);
    }

    std::string_view CinVox::basename(std::string_view path) const {
        const auto pos = path.find_last_of("/\\");
        return (pos == std::string_view::npos) ? path : path.substr(pos + 1);
    }

} //namespace cnx