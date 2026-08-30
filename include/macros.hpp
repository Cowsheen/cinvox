#pragma once

#define TRACE(...)      ::cnx::CinVox::get("default").trace(std::source_location::current(), __VA_ARGS__);
#define DEBUG(...)      ::cnx::CinVox::get("default").debug(std::source_location::current(), __VA_ARGS__);
#define INFO(...)       ::cnx::CinVox::get("default").info(std::source_location::current(), __VA_ARGS__);
#define WARNING(...)    ::cnx::CinVox::get("default").warning(std::source_location::current(), __VA_ARGS__);
#define ERROR(...)      ::cnx::CinVox::get("default").error(std::source_location::current(), __VA_ARGS__);
#define FATAL(...)      ::cnx::CinVox::get("default").fatal(std::source_location::current(), __VA_ARGS__);