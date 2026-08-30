#include "cinvox.hpp"
#include "macros.hpp"

namespace loggers {
    inline constexpr std::string_view GUI = "GUI";
    inline constexpr std::string_view BACKEND = "BACKEND";
}

int main() {
    cnx::CinVox::get(loggers::GUI)
        .set_file_output("log.txt")
        .set_min_log_level(cnx::LogLevel::Info)
        .set_console_output(true);

    cnx::CinVox::get(loggers::BACKEND)
        .set_file_output("log.txt")
        .set_min_log_level(cnx::LogLevel::Debug)
        .set_console_output(true);


    auto& gui = cnx::CinVox::get(loggers::GUI);
    auto& backend = cnx::CinVox::get(loggers::BACKEND);

    std::jthread ui_thread([&] {
        gui.info("window created ({}x{})", 800, 600);
        gui.warning("widget '{}' not found", "btn_save");
    });

    std::jthread backend_thread([&] {
        for (int i = 0; i < 3; ++i)
            backend.debug("query {} executed in {} ms", i, i * 3);
        backend.error("connection to database lost");
    });

    INFO(loggers::GUI, "button '{}' clicked", "OK");
    ERROR(loggers::BACKEND, "retry {} failed", 3);

    cnx::CinVox::shutdown_all();

    return 0;
}