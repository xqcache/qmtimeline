cmake_minimum_required(VERSION 3.23.0 FATAL_ERROR)
set(TARGET_NAME qmtimeline)
project(${TARGET_NAME} VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(QMTIMELINE_JSON_EXTERNAL "Use external nlohmann_json" OFF)
option(QMTIMELINE_BUILD_TESTS "Building test" OFF)
option(QMTIMELINE_BUILD_SHARED_LIBS "" ON)
option(QMTIMELINE_INSTALL "" ON)

add_subdirectory(depends)
add_subdirectory(source)

if(QMTIMELINE_BUILD_TESTS)
    add_subdirectory(tests)
endif()