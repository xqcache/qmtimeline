#pragma once

#if !defined(TIMELINE_BUILD_STATIC)

#ifdef TIMELINE_COMPILE_LIB
#define TIMELINE_LIB_EXPORT __declspec(dllexport)
#else
#define TIMELINE_LIB_EXPORT __declspec(dllimport)
#endif

#else
#define TIMELINE_LIB_EXPORT
#endif
