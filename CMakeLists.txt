cmake_minimum_required(VERSION 3.21)

project(cinvox VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

set(SOURCES
    src/cinvox.cpp
    src/cinvox_config.cpp
)

add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(${PROJECT_NAME} PUBLIC include)