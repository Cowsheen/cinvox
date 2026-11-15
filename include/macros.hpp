#pragma once

#ifndef CINVOX_LOGGER
#define CINVOX_LOGGER "default"
#endif

#define TRACE(...)      ::cnx::CinVox::get(CINVOX_LOGGER).trace(std::source_location::current(), __VA_ARGS__);
#define DEBUG(...)      ::cnx::CinVox::get(CINVOX_LOGGER).debug(std::source_location::current(), __VA_ARGS__);
#define INFO(...)       ::cnx::CinVox::get(CINVOX_LOGGER).info(std::source_location::current(), __VA_ARGS__);
#define WARNING(...)    ::cnx::CinVox::get(CINVOX_LOGGER).warning(std::source_location::current(), __VA_ARGS__);
#define ERROR(...)      ::cnx::CinVox::get(CINVOX_LOGGER).error(std::source_location::current(), __VA_ARGS__);
#define FATAL(...)      ::cnx::CinVox::get(CINVOX_LOGGER).fatal(std::source_location::current(), __VA_ARGS__);