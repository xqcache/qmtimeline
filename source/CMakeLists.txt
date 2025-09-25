set(TARGET_NAME ${PROJECT_NAME})

find_package(QT NAMES Qt6 CONFIG REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} CONFIG REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(PRIVATE_SOURCES
    qmtimeline_global.h
    qmtimelineserializable.h
    qmtimelinelog.h
    qmtimelinetype.h
    qmtimelineview.h
    qmtimelineview.cpp
    qmtimelinescene.h
    qmtimelinescene.cpp
    qmtimelineitem.h
    qmtimelineitem.cpp
    qmtimelineitemmodel.h
    qmtimelineitemmodel.cpp
    qmtimelineitemfactory.h
    qmtimelineitemfactory.cpp
    qmtimelineitemview.h
    qmtimelineitemview.cpp
    qmtimelineitemconnview.h
    qmtimelineitemconnview.cpp
    qmtimelineutil.h
    qmtimelineutil.cpp
    qmtimelinetransaction.h
    qmtimelinetransaction.cpp
)

set(_public_defines "")
set(_private_defines "")

if(QMTIMELINE_BUILD_SHARED OR BUILD_SHARED_LIBS)
    set(_library_type SHARED)
    set(_private_defines QMTIMELINE_COMPILE_LIB)
else()
    set(_library_type STATIC)
    set(_public_defines QMTIMELINE_BUILD_STATIC)
endif()

if(NOT TARGET spdlog)
    message(FATAL_ERROR "=== FATAL ERROR: ${TARGET_NAME} requires spdlog. Please install spdlog.")
endif()

if(NOT TARGET nlohmann_json)
    message(FATAL_ERROR "=== FATAL ERROR: ${TARGET_NAME} requires nlohmann_json. Please install nlohmann_json")
endif()

add_library(${TARGET_NAME} ${_library_type})

set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "d")

target_compile_definitions(${TARGET_NAME} PUBLIC ${_public_defines} PRIVATE ${_private_defines})

target_sources(${TARGET_NAME} PRIVATE ${PRIVATE_SOURCES})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)
target_link_libraries(${TARGET_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Widgets nlohmann_json PRIVATE spdlog::spdlog)
target_include_directories(${TARGET_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>" INTERFACE "$<INSTALL_INTERFACE:include>")

add_subdirectory(widgets)

if(QMTIMELINE_INSTALL)
    include(GNUInstallDirs)
    install(TARGETS spdlog nlohmann_json EXPORT ${TARGET_NAME}Targets RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    install(TARGETS ${TARGET_NAME} EXPORT ${TARGET_NAME}Targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET_NAME} FILES_MATCHING PATTERN "*.h")
    install(EXPORT ${TARGET_NAME}Targets
        FILE ${TARGET_NAME}Tartgets.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_NAME}
        EXPORT_LINK_INTERFACE_LIBRARIES
    )

    include(CMakePackageConfigHelpers)
    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}Config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake/${TARGET_NAME}"
    )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}Config.cmake" DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_NAME})
endif()