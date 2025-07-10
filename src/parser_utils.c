/* SPDX-License-Identifier: Apache-2.0 */
/**
 * @file parser_utils.c
 * @brief Implementation of parsing utilities.
 */

#include "parser_utils.h"
#include <limits.h>
#include <string.h>

/*--------------------------------------------------------------------------------------------------
 *  Internal Helpers
 *------------------------------------------------------------------------------------------------*/

/**
 * @brief Converts an uppercase ASCII letter to lowercase.
 *
 * Only converts 'A' to 'Z' → 'a' to 'z'. All other characters are returned unchanged.
 *
 * @param[in]  c  Input ASCII character
 * @return     Lowercase equivalent if input is 'A'-'Z'; otherwise same character
 */
static char ascii_tolower(char c)
{
    return (('A' <= c) && ('Z' >= c)) ? (char)(c + ('a' - 'A')) : c;
}

/**
 * @brief Converts an ASCII character to its decimal digit value (0–9).
 *
 * Only accepts characters '0' to '9'. Returns false on any other input
 * or if the output pointer is NULL.
 *
 * @param[in]  c     ASCII character to convert
 * @param[out] out   Pointer to store the digit value
 *
 * @retval true   Valid digit and value assigned to out
 * @retval false  Not a digit or out is NULL
 */
static bool ascii_to_digit(char c, uint8_t *out)
{
    if (NULL == out)
    {
        return false;
    }

    if (('0' <= c) && (c <= '9'))
    {
        *out = (uint8_t)(c - '0');
        return true;
    }

    return false;
}

/**
 * @brief Converts an ASCII hex character to its 4-bit binary value.
 *
 * Accepts:
 *  - '0' to '9'
 *  - 'a' to 'f' or 'A' to 'F' (case-insensitive)
 *
 * @param[in]  c     Input ASCII character
 * @param[out] out   Pointer to output (0–15)
 *
 * @retval true   Valid hex character and output assigned
 * @retval false  Invalid character or null pointer
 */
static bool ascii_to_hex(char c, uint8_t *out)
{
    if (NULL == out)
    {
        return false;
    }

    char lower = ascii_tolower(c); /* Converts 'A'–'F' to 'a'–'f' */

    if (('0' <= lower) && (lower <= '9'))
    {
        *out = (uint8_t)(lower - '0');
    }
    else if (('a' <= lower) && (lower <= 'f'))
    {
        *out = (uint8_t)(10u + (uint8_t)(lower - 'a'));
    }
    else
    {
        return false; /* Not a valid hex digit */
    }

    return true;
}

/*--------------------------------------------------------------------------------------------------
 *  Unsigned Integer Parsers
 *------------------------------------------------------------------------------------------------*/

bool parse_u8(const char *s, uint8_t *out)
{
    uint32_t value = 0u;

    if ((NULL == s) || (NULL == out))
    {
        return false;
    }

    if ('\0' == *s)
    {
        return false; /* Reject empty string */
    }

    while ('\0' != *s)
    {
        uint8_t digit;

        if (!ascii_to_digit(*s, &digit))
        {
            return false; /* Non-digit character */
        }

        if (value > (UINT8_MAX / 10u))
        {
            return false; /* Multiplying by 10 would overflow */
        }

        value *= 10u;

        if (digit > (UINT8_MAX - value))
        {
            return false; /* Adding digit would overflow */
        }

        value += digit;
        ++s;
    }

    *out = (uint8_t)value;
    return true;
}

bool parse_u16(const char *s, uint16_t *out)
{
    uint32_t value = 0u;

    if ((NULL == s) || (NULL == out))
    {
        return false;
    }

    if ('\0' == *s)
    {
        return false; /* Reject empty string */
    }

    while ('\0' != *s)
    {
        uint8_t digit;

        if (!ascii_to_digit(*s, &digit))
        {
            return false;
        }

        if (value > (UINT16_MAX / 10u))
        {
            return false; /* Multiplying by 10 would overflow */
        }

        value *= 10u;

        if (digit > (UINT16_MAX - value))
        {
            return false; /* Adding digit would overflow */
        }

        value += digit;
        ++s;
    }

    *out = (uint16_t)value;
    return true;
}

bool parse_u32(const char *s, uint32_t *out)
{
    uint64_t value = 0u;

    if ((NULL == s) || (NULL == out))
    {
        return false;
    }

    if ('\0' == *s)
    {
        return false; /* Reject empty string */
    }

    while ('\0' != *s)
    {
        uint8_t digit;

        if (!ascii_to_digit(*s, &digit))
        {
            return false;
        }

        if (value > (UINT32_MAX / 10u))
        {
            return false; /* Multiplying by 10 would overflow */
        }

        value *= 10u;

        if (digit > (UINT32_MAX - value))
        {
            return false; /* Adding digit would overflow */
        }

        value += digit;
        ++s;
    }

    *out = (uint32_t)value;
    return true;
}

bool parse_u64(const char *s, uint64_t *out)
{
    uint64_t value = 0u;

    if ((NULL == s) || (NULL == out))
    {
        return false;
    }

    if ('\0' == *s)
    {
        return false; /* Reject empty string */
    }

    while ('\0' != *s)
    {
        uint8_t digit;

        if (!ascii_to_digit(*s, &digit))
        {
            return false;
        }

        if (value > (UINT64_MAX / 10u))
        {
            return false; /* Multiplying by 10 would overflow */
        }

        value *= 10u;

        if (digit > (UINT64_MAX - value))
        {
            return false; /* Adding digit would overflow */
        }

        value += digit;
        ++s;
    }

    *out = value;
    return true;
}

/*--------------------------------------------------------------------------------------------------
 *  Signed Integer Parsers
 *------------------------------------------------------------------------------------------------*/

bool parse_i8(const char *s, int8_t *out)
{
    int32_t tmp = 0;

    if ((NULL == s) || (NULL == out))
    {
        return false;
    }

    if (!parse_i32(s, &tmp))
    {
        return false;
    }

    if ((tmp < INT8_MIN) || (tmp > INT8_MAX))
    {
        return false;
    }

    *out = (int8_t)tmp;
    return true;
}

bool parse_i16(const char *s, int16_t *out)
{
    int32_t tmp = 0;

    if ((NULL == s) || (NULL == out))
    {
        return false;
    }

    if (!parse_i32(s, &tmp))
    {
        return false;
    }

    if ((tmp < INT16_MIN) || (tmp > INT16_MAX))
    {
        return false;
    }

    *out = (int16_t)tmp;
    return true;
}

bool parse_i32(const char *s, int32_t *out)
{
    bool negative = false;
    uint32_t value = 0u;

    if ((NULL == s) || (NULL == out))
    {
        return false;
    }

    /* 
     * Check for optional leading sign:
     *  - If '-' found, mark negative and advance pointer
     *  - If '+' found, just advance pointer
     *  - Otherwise, no sign, continue parsing digits
     */
    if ('-' == *s)
    {
        negative = true;
        ++s;
    }
    else if ('+' == *s)
    {
        ++s;
    }
    else
    {
        /* No sign, continue parsing digits */
    }

    if ('\0' == *s)
    {
        return false; /* Reject just "-" or empty input */
    }

    while ('\0' != *s)
    {
        uint8_t digit;

        if (!ascii_to_digit(*s, &digit))
        {
            return false; /* Invalid character */
        }

        /* Compute max allowed value before multiply to avoid overflow:
         * For negative: max = INT32_MAX + 1 (i.e. -2147483648)
         * For positive: max = INT32_MAX (i.e.  2147483647)
         */
        uint32_t max_before_mul = (negative != false)
                                  ? ((uint32_t)INT32_MAX + 1u)
                                  : ((uint32_t)INT32_MAX);

        if (value > (max_before_mul / 10u))
        {
            return false;
        }

        value *= 10u;

        if (digit > (max_before_mul - value))
        {
            return false;
        }

        value += digit;
        ++s;
    }

    *out = (negative != false) ? -(int32_t)value : (int32_t)value;
    return true;
}

bool parse_i64(const char *s, int64_t *out)
{
    bool negative = false;
    uint64_t value = 0u;

    if ((NULL == s) || (NULL == out))
    {
        return false;
    }

    /* 
     * Check for optional leading sign:
     *  - If '-' found, mark negative and advance pointer
     *  - If '+' found, just advance pointer
     *  - Otherwise, no sign, continue parsing digits
     */
    if ('-' == *s)
    {
        negative = true;
        ++s;
    }
    else if ('+' == *s)
    {
        ++s;
    }
    else
    {
        /* No sign, continue parsing digits */
    }

    if ('\0' == *s)
    {
        return false; /* Reject empty string or just '-' */
    }

    while ('\0' != *s)
    {
        uint8_t digit;

        if (!ascii_to_digit(*s, &digit))
        {
            return false; /* Invalid character */
        }

        /* Determine max allowed value before multiplication */
        uint64_t max_before_mul = (negative != false)
                                  ? ((uint64_t)INT64_MAX + 1u)
                                  : ((uint64_t)INT64_MAX);

        if (value > (max_before_mul / 10u))
        {
            return false;
        }

        value *= 10u;

        if (digit > (max_before_mul - value))
        {
            return false;
        }

        value += digit;
        ++s;
    }

    *out = (negative != false) ? -(int64_t)value : (int64_t)value;
    return true;
}

/*--------------------------------------------------------------------------------------------------
 *  Boolean and Hex String Parsers
 *------------------------------------------------------------------------------------------------*/

bool parse_boolean_token(const char *token, bool *out_b)
{
    if ((NULL == token) || (NULL == out_b))
    {
        return false;
    }

    /* Convert manually for case-insensitive string comparison */
    size_t i = 0u;

    /* Check for "true" (case-insensitive) */
    const char true_lit[] = "true";
    for (i = 0u; true_lit[i] != '\0'; ++i)
    {
        if (ascii_tolower(token[i]) != true_lit[i])
        {
            break;
        }
    }
    if ((true_lit[i] == '\0') && (token[i] == '\0'))
    {
        *out_b = true;
        return true;
    }

    /* Check for "false" (case-insensitive) */
    const char false_lit[] = "false";
    for (i = 0u; false_lit[i] != '\0'; ++i)
    {
        if (ascii_tolower(token[i]) != false_lit[i])
        {
            break;
        }
    }
    if ((false_lit[i] == '\0') && (token[i] == '\0'))
    {
        *out_b = false;
        return true;
    }

    /* Exact string match for "1" or "0" */
    if (0 == strcmp(token, "1"))
    {
        *out_b = true;
        return true;
    }

    if (0 == strcmp(token, "0"))
    {
        *out_b = false;
        return true;
    }

    return false;
}

bool parse_hex_string(const char *s, uint8_t *out_buf, size_t max_len, size_t *out_len)
{
    size_t len = 0u;

    if ((NULL == s) || (NULL == out_buf) || (NULL == out_len))
    {
        return false;
    }

    /* Count length of input string */
    while (s[len] != '\0')
    {
        ++len;
    }

    /* Reject empty input string */
    if (len == 0u)
    {
        return false;
    }

    /* Validate that input string length is even: 2 hex characters per byte */
    if ((len & 1u) != 0u)
    {
        return false;
    }

    /* Check if decoded byte count (len / 2) exceeds output buffer capacity */
    if ((len >> 1u) > max_len)
    {
        return false;
    }

    /* Convert each pair of hex characters (2 chars per byte) into a byte */
    for (size_t i = 0u; i < (len >> 1u); ++i)
    {
        uint8_t hi = 0u;
        uint8_t lo = 0u;

        /* Convert high nibble (first hex char of byte) */
        if (!ascii_to_hex(s[i * 2u], &hi))
        {
            return false;
        }

        /* Convert low nibble (second hex char of byte) */
        if (!ascii_to_hex(s[(i * 2u) + 1u], &lo))
        {
            return false;
        }

        /* Combine high and low nibble into one byte */
        out_buf[i] = (uint8_t)((hi << 4u) | lo);
    }

    /* Report the number of bytes written */
    *out_len = len >> 1u;

    return true;
}
