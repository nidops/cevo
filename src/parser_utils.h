/* SPDX-License-Identifier: Apache-2.0 */
/**
 * @file parser_utils.h
 * @brief Low-level parsing utilities for integers, booleans, and hex strings.
 */

#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------------------------------
 *  Unsigned Integer Parsers
 *------------------------------------------------------------------------------------------------*/

/**
 * @brief Parses an ASCII decimal string into a uint8_t value.
 *
 * Accepts only unsigned decimal digits (0–255). Returns false if:
 * - String is NULL or empty
 * - Contains non-digit characters
 * - Overflows 8-bit range during parsing
 *
 * @param[in]  s     Null-terminated input string (e.g., "42")
 * @param[out] out   Pointer to output uint8_t
 *
 * @retval true   Successfully parsed
 * @retval false  On NULL input, invalid chars, or overflow
 */
bool parse_u8(const char *s, uint8_t *out);

/**
 * @brief Parses an ASCII decimal string into a uint16_t value.
 *
 * Accepts only unsigned decimal digits (0–65535). Returns false if:
 * - String is NULL or empty
 * - Contains non-digit characters
 * - Would overflow a 16-bit unsigned int
 *
 * @param[in]  s     Null-terminated input string (e.g., "12345")
 * @param[out] out   Pointer to output uint16_t
 *
 * @retval true   Successfully parsed
 * @retval false  On NULL input, invalid characters, or overflow
 */
bool parse_u16(const char *s, uint16_t *out);

/**
 * @brief Parses an ASCII decimal string into a uint32_t value.
 *
 * Accepts only unsigned decimal digits (0–4294967295). Returns false if:
 * - String is NULL or empty
 * - Contains non-digit characters
 * - Value exceeds uint32_t range during parsing
 *
 * @param[in]  s     Null-terminated input string (e.g., "123456")
 * @param[out] out   Pointer to output uint32_t
 *
 * @retval true   Successfully parsed
 * @retval false  On NULL input, invalid characters, or overflow
 */
bool parse_u32(const char *s, uint32_t *out);

/**
 * @brief Parses an ASCII decimal string into a uint64_t value.
 *
 * Accepts only decimal digits representing an unsigned 64-bit integer.
 * Returns false on:
 *  - NULL input
 *  - empty string
 *  - non-digit characters
 *  - numeric overflow
 *
 * @param[in]  s     Null-terminated decimal string
 * @param[out] out   Pointer to output uint64_t
 *
 * @retval true   Input parsed successfully
 * @retval false  On invalid input or overflow
 */
bool parse_u64(const char *s, uint64_t *out);

/*--------------------------------------------------------------------------------------------------
 *  Signed Integer Parsers
 *------------------------------------------------------------------------------------------------*/

/**
 * @brief Parses an ASCII decimal string into an int8_t value.
 *
 * Uses a full 32-bit signed parse, then verifies the result fits
 * within the 8-bit signed range (-128 to 127).
 *
 * @param[in]  s     Null-terminated string (may begin with '-' or '+')
 * @param[out] out   Pointer to store parsed int8_t result
 *
 * @retval true   Successfully parsed and within int8_t range
 * @retval false  On NULL input, invalid format, or out-of-range value
 */
bool parse_i8(const char *s, int8_t *out);

/**
 * @brief Parses an ASCII decimal string into an int16_t value.
 *
 * Uses a full 32-bit signed parse, then verifies the result fits
 * within the 16-bit signed integer range (-32768 to 32767).
 *
 * @param[in]  s     Null-terminated string (may begin with '-' or '+')
 * @param[out] out   Pointer to store parsed int16_t result
 *
 * @retval true   Successfully parsed and within int16_t range
 * @retval false  On NULL input, invalid format, or out-of-range value
 */
bool parse_i16(const char *s, int16_t *out);

/**
 * @brief Parses a decimal string into an int32_t value.
 *
 * Accepts optional leading minus (`-`) and parses numeric digits.
 * Overflow and underflow are strictly checked during conversion.
 *
 * @param[in]  s     Null-terminated input string (e.g., "-12345")
 * @param[out] out   Pointer to output int32_t value
 *
 * @retval true   Successfully parsed within int32_t range
 * @retval false  NULL pointer, invalid character, or overflow
 */
bool parse_i32(const char *s, int32_t *out);

/**
 * @brief Parses a decimal string into an int64_t value.
 *
 * Accepts optional leading minus ('-') and parses digits into a signed 64-bit integer.
 * Prevents overflow by checking before multiplication and addition steps.
 *
 * @param[in]  s     Null-terminated input string (e.g., "-9223372036854775808")
 * @param[out] out   Pointer to store the parsed int64_t value
 *
 * @retval true   Successfully parsed and within int64_t range
 * @retval false  NULL input, invalid characters, or overflow
 */
bool parse_i64(const char *s, int64_t *out);

/*--------------------------------------------------------------------------------------------------
 *  Boolean and Hex String Parsers
 *------------------------------------------------------------------------------------------------*/

/**
 * @brief Parses a string into a boolean value.
 *
 * Accepts the following (case-insensitive):
 * - "true"  or "1" → true
 * - "false" or "0" → false
 *
 * @param[in]  token   Null-terminated string input (e.g., "true", "0")
 * @param[out] out_b   Pointer to boolean result
 *
 * @retval true   Recognized token and set result
 * @retval false  NULL pointer or invalid string
 */
bool parse_boolean_token(const char *token, bool *out_b);

/**
 * @brief Parses a hexadecimal string into a binary buffer.
 *
 * Converts a null-terminated string containing only hexadecimal characters
 * (e.g., "A0FF12") into a binary buffer.
 *
 * Requirements:
 * - String must have even number of characters.
 * - Max output length must not be exceeded.
 *
 * @param[in]  s         Pointer to input hex string (e.g., "1A2B").
 * @param[out] out_buf   Pointer to output buffer to store parsed bytes.
 * @param[in]  max_len   Maximum number of bytes that can be written to out_buf.
 * @param[out] out_len   Length of parsed bytes.
 *
 * @retval true   Parsing succeeded.
 * @retval false  Invalid input string, null pointers, or output overflow.
 */
bool parse_hex_string(const char *s, uint8_t *out_buf, size_t max_len, size_t *out_len);

#ifdef __cplusplus
}
#endif

#endif /* PARSER_UTILS_H */
