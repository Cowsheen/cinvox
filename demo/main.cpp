#include "cinvox.hpp"
#include "macros.hpp"

int main() {

    auto& logger = cnx::CinVox::instance();
    logger.set_file_output("log.txt");

    logger.trace("direct call without location tracking");

    auto location = std::source_location::current();
    logger.trace(location, "explicit location: {}:{}", location.file_name(), location.line());


    TRACE("Trace from macro")
    DEBUG("Debug from macro")
    INFO("Info from macro")
    WARNING("Warning from macro")
    ERROR("Error from macro")
    FATAL("Fatal from macro")

    return 0;
}