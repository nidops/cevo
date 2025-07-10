/* SPDX-License-Identifier: Apache-2.0 */
/**
 * @file ce_dispatch.h
 * @brief Command Engine, line parser and runtime dispatcher interface.
 *
 * This module provides the main entry point to the CEVO command engine,
 * responsible for parsing a space-delimited input line, validating
 * argument types, and invoking the appropriate command handler.
 */

#ifndef CE_DISPATCH_H
#define CE_DISPATCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Dispatch a command line to the appropriate handler.
 *
 * Parses a space-delimited line (e.g., `"cmd arg1 arg2 ..."`), validates
 * argument types against the CEVO command signature table, and invokes the
 * registered handler if a match is found.
 *
 * @param[in] line_str  Null-terminated command string (not modified).
 *
 * @retval true   Dispatch and execution succeeded.
 * @retval false  Command not found or arguments invalid.
 */
bool ce_dispatch_from_line(const char *line_str);

#ifdef __cplusplus
}
#endif

#endif /* CE_DISPATCH_H */
