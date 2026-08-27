#pragma once

#define TRACE(...)      ::cnx::CinVox::instance().trace(__VA_ARGS__);
#define DEBUG(...)      ::cnx::CinVox::instance().debug(__VA_ARGS__);
#define INFO(...)       ::cnx::CinVox::instance().info(__VA_ARGS__);
#define WARNING(...)    ::cnx::CinVox::instance().warning(__VA_ARGS__);
#define ERROR(...)      ::cnx::CinVox::instance().error(__VA_ARGS__);
#define FATAL(...)      ::cnx::CinVox::instance().fatal(__VA_ARGS__);