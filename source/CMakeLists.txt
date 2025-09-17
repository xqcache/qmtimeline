set(TARGET_NAME timeline)

find_package(QT NAMES Qt6 CONFIG REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} CONFIG REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(PRIVATE_SOURCES
    timeline_global.h
    timelineview.h
    timelineview.cpp
    timelinescene.h
    timelinescene.cpp
    timelineitemmodel.h
    timelineitemmodel.cpp
)

if(TIMELINE_BUILD_SHARED OR BUILD_SHARED_LIBS)
    add_library(${TARGET_NAME} SHARED)
    target_compile_definitions(${TARGET_NAME} PRIVATE TIMELINE_COMPILE_LIB)
else()
    add_library(${TARGET_NAME} STATIC)
    target_compile_definitions(${TARGET_NAME} PUBLIC TIMELINE_BUILD_STATIC)
endif()

target_sources(${TARGET_NAME} PRIVATE ${PRIVATE_SOURCES})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)
target_link_libraries(${TARGET_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Widgets nlohmann_json PRIVATE spdlog::spdlog)
target_include_directories(${TARGET_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>" INTERFACE "$<INSTALL_INTERFACE:include>")
set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "d")

if(TIMELINE_INSTALL)
    include(GNUInstallDirs)
    install(TARGETS spdlog EXPORT ${TARGET_NAME}Targets RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    install(TARGETS ${TARGET_NAME} EXPORT ${TARGET_NAME}Targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/timeline FILES_MATCHING PATTERN "*.h")
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