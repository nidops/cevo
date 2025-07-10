/* SPDX-License-Identifier: Apache-2.0 */

#ifndef CE_DEMO_H
#define CE_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

bool demo_cat_string(const char* s);
bool demo_cat_byte(uint8_t b);
bool demo_cat_bytes(const uint8_t* data, uint8_t len);
bool demo_void(void);
bool demo_cat_mixed(const uint8_t* u8a1,
                    const uint8_t* u8ad2,
                    const char* s1,
                    const char* s2,
                    const uint8_t* u8a3,
                    int64_t i64,
                    uint32_t u32);

#ifdef __cplusplus
}
#endif

#endif /* CE_DEMO_H */
