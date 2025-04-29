set(TARGET_NAME qmtimeline)

find_package(QT NAMES Qt6 CONFIG REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} CONFIG REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(${TARGET_NAME} STATIC)
target_link_libraries(${TARGET_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Widgets nlohmann_json)

set(PRIVATE_SOURCES
    qmtlgraphicsview.h
    qmtlgraphicsview.cpp
    qmtlgraphicsscene.h
    qmtlgraphicsscene.cpp
    qmtlgraphicsmodel.h
    qmtlgraphicsmodel.cpp
    qmtldatetimeaxis.h
    qmtldatetimeaxis.cpp
)
target_sources(${TARGET_NAME} PRIVATE ${PRIVATE_SOURCES})

add_subdirectory(graphicsitem)
add_subdirectory(itemdata)
add_subdirectory(itemmodel)