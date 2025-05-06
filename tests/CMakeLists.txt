set(TARGET_NAME qmtimeline_test)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE qmtimeline)
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/source)

set(PRIVATE_SOURCES
    tldemoitemdata.h
    tldemoitemdata.cpp
    tldemoitemmodel.h
    tldemoitemmodel.cpp
    tldemoitemprimitive.h
    tldemoitemprimitive.cpp
    tldemoitemregistry.h
    tldemoitemregistry.cpp
)
target_sources(${TARGET_NAME} PRIVATE ${PRIVATE_SOURCES})
