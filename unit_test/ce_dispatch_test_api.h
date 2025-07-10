/* SPDX-License-Identifier: Apache-2.0 */
/**
 * @file ce_dispatch_test_api.h
 * @brief Internal test interface for `ce_dispatch.c`.
 *
 * This header exposes wrapper functions to unit test internal `static`
 * functions of the command engine's dispatcher module.
 *
 * These symbols are *only* available when `UNIT_TEST` is defined.
 *
 * @warning This header is strictly for testing purposes and must not be
 *          included or compiled in production builds.
 */

#ifndef CE_DISPATCH_TEST_API_H
#define CE_DISPATCH_TEST_API_H

#include <stdint.h>
#include <stdbool.h>
#include "ce_types.h"
#include "ce_dispatch.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef UNIT_TEST

/**
 * @brief Test wrapper for validate_argument_count().
 *
 * @param tokens_count Number of tokens parsed.
 * @param sig_pst Pointer to the command signature.
 * @return true if valid, false otherwise.
 */
bool _test_validate_argument_count(int token_count, const ce_signature_st *sig_pst);

/**
 * @brief Test wrapper for parse_arguments().
 *
 * @param sig_pst Pointer to the command signature.
 * @param tokens_ca Array of token strings (command and arguments).
 * @param out_args_a Output array for parsed argument values.
 * @return true if all arguments parsed successfully.
 */
bool _test_parse_arguments(const ce_signature_st *sig_pst, char *tokens_ca[], ce_arg_value_ut out_args_a[MAX_TOKENS]);

/**
 * @brief Test wrapper for parse_line_to_tokens().
 *
 * @param line Input line to be tokenized (modified in-place).
 * @param tokens Output array of token pointers.
 * @param tokens_count Output count of tokens.
 * @return true on success, false on overflow or error.
 */
bool _test_parse_line_to_tokens(char *line, char *tokens[], size_t *token_count);

/**
 * @brief Test wrapper for dispatch_command().
 *
 * @param sig_pst Pointer to the command signature.
 * @param tokens_ca Tokenized input (command + arguments).
 */
void _test_dispatch_command(const ce_signature_st *sig_pst, char *tokens_ca[]);

/**
 * @brief Test wrapper for internal lookup_signature_by_hash().
 *
 * This function is exposed only during unit testing to allow validation
 * of the command lookup logic based on hash values.
 *
 * @param hash 32-bit hash of the command name.
 * @return Pointer to matching ce_signature_st if found, or NULL.
 */
const ce_signature_st* _test_lookup_signature_by_hash(uint32_t hash);

#endif /* UNIT_TEST */

#ifdef __cplusplus
}
#endif

#endif /* CE_DISPATCH_TEST_API_H */
