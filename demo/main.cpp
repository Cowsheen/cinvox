#include "cinvox.hpp"
#include "macros.hpp"

int main() {
    auto& log = cnx::CinVox::instance();

    log.set_file_output("log.txt");
    log.set_min_log_level(cnx::LogLevel::Debug);
    log.set_console_output(true);

    {
        std::vector<std::jthread> pool;
        for (int t = 0; t < 8; ++t) {
            pool.emplace_back([t] {
                for (int i = 0; i < 100; ++i) {
                    INFO("thread {} iteration {}, value = {:.2f}", t, i, i * 1.5);
                    WARNING("thread {} slow iteration {}", t, i);
                    DEBUG("thread {} debug payload {}", t, i * i);
                }
            });
        }
    } 
    
    log.trace("direct call without location tracking");

    auto location = std::source_location::current();
    log.trace(location, "explicit location: {}:{}", location.file_name(), location.line());

    TRACE("Trace from macro")
    DEBUG("Debug from macro")
    INFO("Info from macro")
    WARNING("Warning from macro")
    ERROR("Error from macro")
    FATAL("Fatal from macro")

    return 0;
}