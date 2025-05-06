#pragma once

#include <QtCore/qglobal.h>

#ifndef QMTIMELINE_BUILD_STATIC
#define QMTIMELINE_INITIALIZE_RESOURCE
#if defined(QMTIMELINE_COMPILE_LIB)
#define QMTIMELINE_EXPORT Q_DECL_EXPORT
#else
#define QMTIMELINE_EXPORT Q_DECL_IMPORT
#endif
#else
#define QMTIMELINE_EXPORT
#endif
