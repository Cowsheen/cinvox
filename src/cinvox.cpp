#include "cinvox.hpp"

namespace cnx {
    CinVox& CinVox::instance() {
        static CinVox instance;
        return instance;
    }

    CinVox::CinVox() : m_worker([this](std::stop_token st){ run(st); })
                     , m_min_log_level(LogLevel::Trace)
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
        std::string line = msg.text + '\n'; // TODO

        std::fwrite(line.data(), 1, line.size(), stdout);

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


} //namespace cnx