/* SPDX-License-Identifier: Apache-2.0 */
/**
 * @file ce_hash.c
 * @brief Implementation of DJB2-based hashing.
 */

#include <stddef.h>
#include "ce_hash.h"

/* DJB2 hash constants */
#define DJB2_INIT_HASH        (5381u)
#define DJB2_HASH_SHIFT       (5u)
#define DJB2_HASH_MULTIPLIER  (33u)  /* Because (hash << 5) + hash == hash * 33 */

uint32_t ce_hash_calculate(const char* str)
{
    uint32_t hash = DJB2_INIT_HASH;
    char c = '\0';

    if (NULL == str)
    {
        return 0u;
    }

    while ((c = *str++) != '\0')
    {
        /* Equivalent to hash * 33 + c */
        hash = ((hash << DJB2_HASH_SHIFT) + hash) + (uint32_t)c;
    }

    return hash;
}
