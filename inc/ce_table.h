/* SPDX-License-Identifier: Apache-2.0 */
/**
 * @file ce_table.h
 * @brief Accessors for the auto-generated command dispatch table.
 *
 * This interface allows read-only access to the table of command signatures,
 * which maps hashed command names to their respective handlers and expected argument types.
 */

#ifndef CE_TABLE_H
#define CE_TABLE_H

#include <stddef.h>
#include "ce_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get a pointer to the static signature table.
 *
 * The signature table is defined in `ce_table.c`, auto-generated from YAML.
 * Each entry contains a hash ID, handler function, and type signature.
 *
 * @return Pointer to constant array of `ce_signature_st` entries
 */
const ce_signature_st* ce_table_get_signatures(void);

/**
 * @brief Get the number of registered command entries in the dispatch table.
 *
 * @return The total number of command signature entries
 */
size_t ce_table_get_signatures_count(void);

#ifdef __cplusplus
}
#endif

#endif /* CE_TABLE_H */

