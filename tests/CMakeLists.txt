set(TARGET_NAME qmtimeline_test)

add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE qmtimeline)
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/source)