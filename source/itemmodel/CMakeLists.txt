set(PRIVATE_SOURCES
    qmtlitemmodel_p.h
    qmtlitemmodel.h
    qmtlitemmodel.cpp
    qmtlframeitemmodel.h
    qmtlframeitemmodel.cpp
)

target_sources(${TARGET_NAME} PRIVATE ${PRIVATE_SOURCES})
