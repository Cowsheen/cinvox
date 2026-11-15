#pragma once
#include "types.hpp"

namespace cnx {
    void configure_from_json(const std::string& path);
    std::optional<std::vector<LogConfig>> load_config(const std::string& path);
    void apply_config(const std::optional<std::vector<LogConfig>>& configs);
} // namespace cnx