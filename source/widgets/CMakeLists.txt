set(PRIVATE_SOURCES
    qmtimelineranger.h
    qmtimelineranger.cpp
    qmtimelineranger.ui
    qmtimelinerangeslider.h
    qmtimelinerangeslider.cpp
    qmtimelineaxis.h
    qmtimelineaxis.cpp
)

target_sources(${TARGET_NAME} PRIVATE ${PRIVATE_SOURCES})
