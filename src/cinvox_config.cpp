#include "cinvox_config.hpp"
#include <unordered_set>
#include <fstream>
#include "nlohmann/json.hpp"
#include "cinvox.hpp"
#include <iostream>
namespace cnx {

    std::optional<LogLevel> parse_level(std::string_view s) {
        std::string lower;
        lower.reserve(s.size());
        for(char c: s) {
            lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }

        if (lower == "trace")                           return LogLevel::Trace;
        if (lower == "debug")                           return LogLevel::Debug;
        if (lower == "info")                            return LogLevel::Info;
        if (lower == "warn" || lower == "warning")      return LogLevel::Warning;
        if (lower == "error")                           return LogLevel::Error;
        if (lower == "critical" || lower == "fatal")    return LogLevel::Fatal;
        if (lower == "off" || lower == "none")          return LogLevel::Off;

        return std::nullopt;
    }

    std::string expand_placeholders(std::string pattern, std::string_view name) {
        constexpr std::string_view token = "{name}";
        std::string out{""};
        size_t pos = 0;

        while(true) {
            const size_t found = pattern.find(token, pos);
            if(found == std::string_view::npos) {
                out.append(pattern.substr(pos));
                break;
            }
            out.append(pattern.substr(pos, found - pos));
            out.append(name);
            pos = found + token.size();
        }
        return out;
    }

std::vector<LogConfig> load_config(const std::string& path) {
    std::ifstream in(path);
    if (!in)
        throw std::runtime_error("cannot open config file: " + path);

    nlohmann::json doc;
    in >> doc;                                   // throws on malformed JSON

    // -------- defaults section (all optional) --------
    LogLevel default_level   = LogLevel::Info;
    bool default_console = true;
    std::optional<std::string> default_file;

    if (auto it = doc.find("defaults"); it != doc.end() && it->is_object()) {
        const auto& d = *it;
        if (d.contains("level") && d["level"].is_string()) {
            auto lvl = parse_level(d["level"].get<std::string>());
            if (!lvl)
                throw std::runtime_error("unknown level: " + d["level"].get<std::string>());
            default_level = *lvl;
        }
        if (d.contains("console") && d["console"].is_boolean())
            default_console = d["console"].get<bool>();
        if (d.contains("file") && d["file"].is_string())
            default_file = d["file"].get<std::string>();
    }

    // -------- loggers array (required) --------
    const auto it = doc.find("loggers");
    if (it == doc.end() || !it->is_array())
        throw std::runtime_error("config must contain a 'loggers' array");

    std::vector<LogConfig> out;
    std::unordered_set<std::string> seen;

    for (const auto& entry : *it) {
        if (!entry.is_object() || !entry.contains("name") || !entry["name"].is_string())
            throw std::runtime_error("each logger entry must be an object with a string 'name'");

        const std::string name = entry["name"].get<std::string>();
        if (!seen.insert(name).second)
            throw std::runtime_error("duplicate logger name in config: " + name);

        LogConfig c;
        c.name    = name;
        c.level   = default_level;
        c.console = default_console;
        c.file    = default_file;

        if (entry.contains("level") && entry["level"].is_string()) {
            auto lvl = parse_level(entry["level"].get<std::string>());
            if (!lvl)
                throw std::runtime_error("unknown level for logger '" + name + "'");
            c.level = *lvl;
        }
        if (entry.contains("console") && entry["console"].is_boolean())
            c.console = entry["console"].get<bool>();
        if (entry.contains("file") && entry["file"].is_string())
            c.file = entry["file"].get<std::string>();

        if (c.file && !c.file->empty())
            *c.file = expand_placeholders(*c.file, c.name);

        out.push_back(std::move(c));
    }
    return out;
}

void apply_config(const std::vector<LogConfig>& configs) {
    std::unordered_multiset<std::string> configured;
    configured.reserve(configs.size());

    for (const auto& c : configs) {
        configured.insert(c.name);
    }

    for (const std::string& name : CinVox::names()) {
        if (!configured.contains(name)) {
            CinVox::get(name).set_min_log_level(LogLevel::Off); // TODO figure out what to do with loggers that are not in config file.
        }                                                       // Disabled (level = Off) for now
    }

    for (const auto& c : configs) {
        auto& log = CinVox::get(c.name);
        log.set_min_log_level(c.level)
           .set_console_output(c.console);
        if (c.file && !c.file->empty())
            log.set_file_output(*c.file);
    }
}

void configure_from_json(const std::string& path) {
    apply_config(load_config(path));
}

} // namespace cnx