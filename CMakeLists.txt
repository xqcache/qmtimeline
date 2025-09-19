cmake_minimum_required(VERSION 3.24.0 FATAL_ERROR)

project(qmtimeline VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MAIN_PROJECT OFF)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    set(MAIN_PROJECT ON)
endif()

option(QMTIMELINE_FMT_EXTERNAL "Use external fmt" OFF)
option(QMTIMELINE_JSON_EXTERNAL "Use external json" OFF)
option(QMTIMELINE_SPDLOG_EXTERNAL "Use external spdlog" OFF)
option(QMTIMELINE_BUILD_SHARED "Use external spdlog" ON)
option(QMTIMELINE_BUILD_TESTS "Build tests" ${MAIN_PROJECT})
option(QMTIMELINE_INSTALL "Build tests" ${MAIN_PROJECT})

if(MAIN_PROJECT)
    if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin")
    endif()

    if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib")
    endif()

    if(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib")
    endif()
endif()

add_subdirectory(thirdparty)
add_subdirectory(source)

if(QMTIMELINE_BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    add_subdirectory(tests)
endif()

# 个人私有测试用例（不上代码库）
if(MAIN_PROJECT AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/_tests")
    add_subdirectory(_tests)
endif()