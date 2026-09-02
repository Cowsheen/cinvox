#pragma once
#include "types.hpp"

namespace cnx {

    // std::optional<LogLevel> parse_level(std::string_view s);
    // std::string expand_placeholders(std::string pattern, std::string_view name);
    void configure_from_json(const std::string& path);
} // namespace cnx