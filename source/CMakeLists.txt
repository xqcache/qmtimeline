set(TARGET_NAME qmtimeline)

find_package(QT NAMES Qt6 CONFIG REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} CONFIG REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(QMTIMELINE_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    add_library(${TARGET_NAME} SHARED)
    target_compile_definitions(${TARGET_NAME} PRIVATE QMTIMELINE_COMPILE_LIB)
else()
    add_library(${TARGET_NAME} STATIC)
    target_compile_definitions(${TARGET_NAME} PUBLIC QMTIMELINE_BUILD_STATIC)
endif()

target_link_libraries(${TARGET_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Widgets nlohmann_json)
target_include_directories(${TARGET_NAME} INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")

set(PRIVATE_SOURCES
    qmtlgraphicsview.h
    qmtlgraphicsview.cpp
    qmtlgraphicsscene.h
    qmtlgraphicsscene.cpp
    qmtlgraphicsmodel.h
    qmtlgraphicsmodel.cpp
    qmtldatetimeaxis.h
    qmtldatetimeaxis.cpp
    qmtlitemregistry.h
    qmtlitemdata.h
    qmtlitemdata.cpp
    qmtlitemmodel.h
    qmtlitemmodel.cpp
    qmtlitemprimitive.h
    qmtlitemprimitive.cpp
)
target_sources(${TARGET_NAME} PRIVATE ${PRIVATE_SOURCES})

if(QMTIMELINE_INSTALL)
    install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()