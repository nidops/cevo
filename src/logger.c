/* SPDX-License-Identifier: Apache-2.0 */
/**
 * @file logger.c
 * @brief Implements a minimal logging history buffer for storing recent log events.
 */

#include <stddef.h>
#include "logger.h"

/**
 * @brief Size of the circular buffer storing recent log entries.
 */
#define LOG_HISTORY_SIZE 10u

/**
 * @brief Structure representing a single log entry.
 */
typedef struct
{
    ce_file_id_et file_id; /**< Identifier of the source file */
    uint16_t line;         /**< Line number in the source file */
} log_entry_st;

/* Circular buffer to hold recent log entries */
static log_entry_st log_history[LOG_HISTORY_SIZE];

/* Current index for the next log entry insertion */
static size_t log_history_idx = 0u;

/**
 * @brief Records a log event into the circular log history buffer.
 *
 * Stores the file ID and line number where the event occurred.
 *
 * @param file_id Identifier of the source file generating the log event.
 * @param line    Line number in the source file.
 */
void log_event(ce_file_id_et file_id, uint16_t line)
{
    log_history[log_history_idx].file_id = file_id;
    log_history[log_history_idx].line = line;

    log_history_idx = (log_history_idx + 1u) % LOG_HISTORY_SIZE;
}
