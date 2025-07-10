/* SPDX-License-Identifier: Apache-2.0 */
/**
 * @file ce_types.h
 * @brief Core type definitions for the Command Engine.
 *
 * This header defines essential types used in the runtime dispatch system,
 * including argument types, their encoded values, and macros for extracting
 * raw values in a type-safe, scalable way.
 *
 * Only MISRA-friendly types are supported here.
 */

#ifndef CE_TYPES_H
#define CE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>

#define MAX_TOKENS            (8U)    /**< Maximum number of tokens (command + args) */
#define MAX_ARG_CONTENT_SIZE  (64U)   /**< Max size in bytes for a single string or binary argument */
#define MAX_LINE_BUF_SIZE     (256U)  /**< Max length of the input command line */

/**
 * @enum ce_arg_type_et
 * @brief Enumerates all supported runtime argument types.
 *
 * These types represent how each parsed argument should be interpreted
 * when dispatching to a command handler.
 *
 * Only explicitly-sized MISRA-compliant types are allowed.
 */
typedef enum {
    TYPE_UINT8_e,       /**< 8-bit unsigned integer */
    TYPE_UINT16_e,      /**< 16-bit unsigned integer */
    TYPE_UINT32_e,      /**< 32-bit unsigned integer */
    TYPE_UINT64_e,      /**< 64-bit unsigned integer */
    TYPE_INT8_e,        /**< 8-bit signed integer   */
    TYPE_INT16_e,       /**< 16-bit signed integer  */
    TYPE_INT32_e,       /**< 32-bit signed integer  */
    TYPE_INT64_e,       /**< 64-bit signed integer  */
    TYPE_BOOL_e,        /**< Boolean (0 or 1)       */
    TYPE_STRING_e,      /**< Null-terminated string */
    TYPE_UINT8_PTR_e    /**< Byte buffer (parsed from hex string) */
} ce_arg_type_et;

/**
 * @union ce_arg_value_ut
 * @brief Union representing a parsed command argument value.
 *
 * Only one field is valid at runtime, based on the corresponding type tag.
 * This union avoids heap allocations and supports zero-copy for strings.
 */
typedef union {
    uint8_t         u8;
    uint16_t        u16;
    uint32_t        u32;
    uint64_t        u64;
    int8_t          i8;
    int16_t         i16;
    int32_t         i32;
    int64_t         i64;
    bool            b;
    const char     *str_cp;        /**< Pointer to null-terminated string */
    const uint8_t  *bytes_u8p;     /**< Pointer to raw byte array */
} ce_arg_value_ut;

/**
 * @typedef handler_func_t
 * @brief Generic function pointer (casted at dispatch time).
 */
typedef void (*handler_func_t)(void);

/**
 * @struct ce_signature_st
 * @brief Command signature metadata used for dispatching.
 */
typedef struct {
    uint32_t hash_u32;                 /**< Hashed command name */
    handler_func_t handler;            /**< Function pointer to the command implementation */
    const ce_arg_type_et *types_e;     /**< Array of argument types */
    uint8_t type_count_u8;             /**< Number of arguments expected */
} ce_signature_st;

#ifdef UNIT_TEST

/**
 * @brief Extracts value from union for testing/logging.
 *
 * Used only in unit test mode. This function maps the argument type
 * to the correct member of `ce_arg_value_ut`.
 */
static inline uintptr_t ce_get_arg_val(ce_arg_type_et type, ce_arg_value_ut val)
{
    switch (type) {
        case TYPE_UINT8_e:      return (uintptr_t)val.u8;
        case TYPE_UINT16_e:     return (uintptr_t)val.u16;
        case TYPE_UINT32_e:     return (uintptr_t)val.u32;
        case TYPE_UINT64_e:     return (uintptr_t)val.u64;
        case TYPE_INT8_e:       return (uintptr_t)val.i8;
        case TYPE_INT16_e:      return (uintptr_t)val.i16;
        case TYPE_INT32_e:      return (uintptr_t)val.i32;
        case TYPE_INT64_e:      return (uintptr_t)val.i64;
        case TYPE_BOOL_e:       return (uintptr_t)val.b;
        case TYPE_STRING_e:     return (uintptr_t)val.str_cp;
        case TYPE_UINT8_PTR_e:  return (uintptr_t)val.bytes_u8p;
        default:
            return (uintptr_t)0;
    }
}

#define ARGVAL(type, value) ce_get_arg_val((type), (value))

#else

/**
 * @brief Lightweight macro to extract value from `ce_arg_value_ut`
 * based on type (release mode, no printf fallback).
 */
#define ARGVAL(type, value) (                                               \
        (type) == TYPE_UINT8_e     ? (uintptr_t)(value).u8        :         \
        (type) == TYPE_UINT16_e    ? (uintptr_t)(value).u16       :         \
        (type) == TYPE_UINT32_e    ? (uintptr_t)(value).u32       :         \
        (type) == TYPE_UINT64_e    ? (uintptr_t)(value).u64       :         \
        (type) == TYPE_INT8_e      ? (uintptr_t)(value).i8        :         \
        (type) == TYPE_INT16_e     ? (uintptr_t)(value).i16       :         \
        (type) == TYPE_INT32_e     ? (uintptr_t)(value).i32       :         \
        (type) == TYPE_INT64_e     ? (uintptr_t)(value).i64       :         \
        (type) == TYPE_BOOL_e      ? (uintptr_t)(value).b         :         \
        (type) == TYPE_STRING_e    ? (uintptr_t)(value).str_cp    :         \
        (type) == TYPE_UINT8_PTR_e ? (uintptr_t)(value).bytes_u8p :         \
        (uintptr_t)0)

#endif /* UNIT_TEST */

#ifdef __cplusplus
}
#endif

#endif /* CE_TYPES_H */
