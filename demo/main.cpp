#include "cinvox.hpp"

int main() {

    auto& logger = cnx::CinVox::instance();

    logger.trace("direct call without location tracking");

    auto location = std::source_location::current();
    logger.trace(location, "explicit location: {}:{}", location.file_name(), location.line());

    return 0;
}