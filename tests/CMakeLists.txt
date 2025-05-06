set(TARGET_NAME qmtimeline_test)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_executable(${TARGET_NAME} main.cpp tlitemregistry.cpp tlitemregistry.h)
target_link_libraries(${TARGET_NAME} PRIVATE qmtimeline)
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/source)

add_subdirectory(itemdata)
add_subdirectory(itemmodel)
add_subdirectory(itemprimitive)
