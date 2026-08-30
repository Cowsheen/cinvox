#include "cinvox.hpp"
#include <chrono>

namespace cnx {
    CinVox& CinVox::instance() {
        static CinVox instance;
        return instance;
    }

    CinVox::CinVox() : m_worker([this](std::stop_token st){ run(st); })
                     , m_min_log_level(LogLevel::Trace)
                     , m_console_output(false)
    {}

    CinVox::~CinVox() {
        shutdown();
    }

    void CinVox::set_file_output(std::string_view path) {
        std::lock_guard lock(m_file_mutex);
        m_file.close();
        m_file.open(std::string(path), std::ios::out | std::ios::app);
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
        m_worker.request_stop();
        m_cv.notify_all();
        m_worker.join();
    }

    std::string CinVox::format_line(const LogMessage& msg) const {
        const auto tp =  msg.timestamp;
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) % 1000;

        return std::format("[{:%Y-%m-%d %H:%M:%S}.{:03}] [{}] [{}:{}] {}\n",
                            tp, ms.count(),
                            level_name(msg.level),
                            msg.location.file_name(),
                            msg.location.line(),
                            msg.text);
    }

} //namespace cnx