#pragma once

#define TRACE(...)      ::cnx::CinVox::instance().trace(std::source_location::current(), __VA_ARGS__);
#define DEBUG(...)      ::cnx::CinVox::instance().debug(std::source_location::current(), __VA_ARGS__);
#define INFO(...)       ::cnx::CinVox::instance().info(std::source_location::current(), __VA_ARGS__);
#define WARNING(...)    ::cnx::CinVox::instance().warning(std::source_location::current(), __VA_ARGS__);
#define ERROR(...)      ::cnx::CinVox::instance().error(std::source_location::current(), __VA_ARGS__);
#define FATAL(...)      ::cnx::CinVox::instance().fatal(std::source_location::current(), __VA_ARGS__);