#pragma once

#if !defined(QMTIMELINE_BUILD_STATIC)

#ifdef QMTIMELINE_COMPILE_LIB
#define QMTIMELINE_LIB_EXPORT __declspec(dllexport)
#else
#define QMTIMELINE_LIB_EXPORT __declspec(dllimport)
#endif

#else
#define QMTIMELINE_LIB_EXPORT
#endif
