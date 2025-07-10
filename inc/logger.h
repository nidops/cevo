/* SPDX-License-Identifier: Apache-2.0 */
/**
 * @file logger.h
 * @brief Minimal logging macro with automatic file, line, and function context.
 *
 * Designed for critical systems: collapsible in production builds.
 */

#ifndef CE_LOGGER_H
#define CE_LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @enum ce_file_id_et
 * @brief Unique identifiers for source files used in logging.
 */
typedef enum {
    CE_FILE_DISPATCH_e = 0,
    CE_FILE_PARSER_UTILS_e,
    /* Add other files here */
    CE_FILE_MAX_e
} ce_file_id_et;

/**
 * @brief Stores a log entry with file ID and line number in a circular buffer.
 *
 * This allows later inspection of recent error locations even if printing
 * is disabled (e.g., in release builds).
 *
 * @param file_id Unique file ID identifying the source file.
 * @param line    Line number in the source file where the log call was made.
 */
void log_event(ce_file_id_et file_id, uint16_t line);

#if defined(UNIT_TEST) || defined(CE_LOG_ENABLE)

#include <stdio.h>

#if defined(__GNUC__)
static inline const char *get_filename(const char *path)
{
    const char *p = __builtin_strrchr(path, '/');
    return p ? p + 1 : path;
}
#else
static inline const char *get_filename(const char *path)
{
    return path;
}
#endif

#define ANSI_COLOR_BRIGHT_RED    "\x1b[1;31m"
#define ANSI_COLOR_RED           "\x1b[31m"
#define ANSI_COLOR_BRIGHT_CYAN   "\x1b[1;36m"
#define ANSI_COLOR_CYAN          "\x1b[36m"
#define ANSI_COLOR_BRIGHT_YELLOW "\x1b[1;33m"
#define ANSI_COLOR_YELLOW        "\x1b[33m"
#define ANSI_COLOR_RESET         "\x1b[0m"

/**
 * @brief Logs an error message with file, line, and function context in red text.
 *
 * Automatically inserts __FILE__, __LINE__, and __func__.
 *
 * @param file_id Unique file identifier.
 * @param fmt printf-style format string.
 * @param ... Arguments for format string.
 */
#define LOG_ERR(file_id, fmt, ...)                                                  \
    do {                                                                            \
        log_event(file_id, (uint16_t)__LINE__);                                     \
        printf(                                                                     \
            ANSI_COLOR_BRIGHT_RED "[ERR]" ANSI_COLOR_RESET                          \
            ANSI_COLOR_CYAN "[%s:%u:%s]" ANSI_COLOR_RESET " "                       \
            ANSI_COLOR_RED "❌" ANSI_COLOR_RESET " "                                \
            fmt "\n",                                                               \
            get_filename(__FILE__), (unsigned)__LINE__, __func__, ##__VA_ARGS__);   \
    } while (0)

/**
 * @brief Logs a debug message with file, line, and function context in bright cyan text.
 *
 * Automatically inserts __FILE__, __LINE__, and __func__.
 *
 * @param file_id Unique file identifier.
 * @param fmt printf-style format string.
 * @param ... Arguments for format string.
 */
#define LOG_DBG(file_id, fmt, ...)                                                  \
    do {                                                                            \
        printf(                                                                     \
            ANSI_COLOR_BRIGHT_YELLOW "[DBG]" ANSI_COLOR_RESET                       \
            ANSI_COLOR_CYAN "[%s:%u:%s]" ANSI_COLOR_RESET " "                       \
            ANSI_COLOR_RESET                                                        \
            fmt "\n",                                                               \
            get_filename(__FILE__), (unsigned)__LINE__, __func__, ##__VA_ARGS__);   \
    } while (0)

#else

/**
 * @brief Stores log entry in history buffer; no print in release builds.
 */
#define LOG_ERR(file_id, fmt, ...)              \
    do {                                        \
        log_event(file_id, (uint16_t)__LINE__); \
        /* No print in release */               \
    } while (0)

/**
 * @brief Debug logs do not store events in release builds.
 */
#define LOG_DBG(file_id, fmt, ...)    (void)0;

#endif /* UNIT_TEST || CE_LOG_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* CE_LOGGER_H */
