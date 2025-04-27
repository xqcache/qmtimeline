cmake_minimum_required(VERSION 3.23.0 FATAL_ERROR)
set(TARGET_NAME qmtimeline)
project(${TARGET_NAME} VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(source)
add_subdirectory(test)