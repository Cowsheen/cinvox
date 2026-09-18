#include "cinvox.hpp"
#include "cinvox_config.hpp"
#include "macros.hpp"

namespace loggers {
    inline constexpr std::string_view GUI = "GUI";
    inline constexpr std::string_view BACKEND = "BACKEND";
}

int main() {
    cnx::init("config.json");

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

    INFO("button '{}' clicked", "OK");
    ERROR("retry {} failed", 3);

    for (int i = 0; i < 600; ++i) { // Watcher demo
        gui.debug("frame {}", i);
        backend.debug("tick {}", i);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    return 0;
}