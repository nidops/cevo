/* SPDX-License-Identifier: Apache-2.0 */
/**
 * @file ce_hash.h
 * @brief Implementation of DJB2-based hashing for cevo.
 */

#ifndef CE_HASH_H
#define CE_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief DJB2 hash algorithm implementation (32-bit).
 *
 * This hashing function is used to convert string command names
 * into consistent 32-bit hash values at runtime.
 *
 * It is compatible with the same hashing logic implemented in Python
 * for command IDs generation during build time.
 *
 * @param str Null-terminated input string to hash
 * @return Unsigned 32-bit hash result
 */
uint32_t ce_hash_calculate(const char* str);

#ifdef __cplusplus
}
#endif

#endif /* CE_HASH_H */
