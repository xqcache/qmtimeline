#pragma once

#ifndef QMTL_LOG_CRITICAL
#ifndef SPDLOG_H
#include <spdlog/spdlog.h>
#endif
#define QMTL_LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(spdlog::default_logger_raw(), __VA_ARGS__)
#endif

#ifndef QMTL_LOG_ERROR
#ifndef SPDLOG_H
#include <spdlog/spdlog.h>
#endif
#define QMTL_LOG_ERROR(...) SPDLOG_LOGGER_ERROR(spdlog::default_logger_raw(), __VA_ARGS__)
#endif

#ifndef QMTL_LOG_WARN
#ifndef SPDLOG_H
#include <spdlog/spdlog.h>
#endif
#define QMTL_LOG_WARN(...) SPDLOG_LOGGER_WARN(spdlog::default_logger_raw(), __VA_ARGS__)
#endif

#ifndef QMTL_LOG_INFO
#ifndef SPDLOG_H
#include <spdlog/spdlog.h>
#endif
#define QMTL_LOG_INFO(...) SPDLOG_LOGGER_INFO(spdlog::default_logger_raw(), __VA_ARGS__)
#endif

#ifndef QMTL_LOG_DEBUG
#ifndef SPDLOG_H
#include <spdlog/spdlog.h>
#endif
#define QMTL_LOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(spdlog::default_logger_raw(), __VA_ARGS__)
#endif
