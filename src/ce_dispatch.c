/* SPDX-License-Identifier: Apache-2.0 */
/**
 * @file ce_dispatch.c
 * @brief Implements the runtime command dispatcher with parsing and validation.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#include "parser_utils.h"

#include "logger.h"
#include "ce_table.h"
#include "ce_command_ids.h"
#include "ce_invoke_handler.h"
#include "ce_dispatch.h"
#include "ce_hash.h"

typedef struct {
    uint8_t buffer[MAX_ARG_CONTENT_SIZE];
    size_t used;
} scratch_buffer_st;


#define IS_WHITESPACE(c) ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n')

#define ARG_OFFSET             (1u)  /**< Offset to skip command name in tokens_ca[] */
#define MAX_PARSABLE_ARGUMENTS (MAX_TOKENS - ARG_OFFSET)


/**
 * @brief Validates that the number of arguments matches the signature.
 *
 * @param tokens_count Number of tokens parsed from the line (including command name).
 * @param sig_pst Pointer to the command signature.
 * @return true if count is valid, false otherwise.
 */
static bool validate_argument_count(size_t tokens_count, const ce_signature_st *sig_pst)
{
    if (NULL == sig_pst)
    {
        LOG_ERR(CE_FILE_DISPATCH_e, "Null pointer");
        return false;
    }

    size_t arg_count = (tokens_count > 0u) ? (tokens_count - 1u) : 0u;

    if (arg_count != sig_pst->type_count_u8)
    {
        LOG_ERR(CE_FILE_DISPATCH_e,
                "Argument count mismatch (got %lu, expected %u)",
                arg_count, sig_pst->type_count_u8);
        return false;
    }

    return true;
}

static bool parse_value_by_type(ce_arg_type_et type_e,
                                const char *token_cp,
                                ce_arg_value_ut *out_val_p,
                                scratch_buffer_st *scratch)
{
    if ((NULL == token_cp) || (NULL == out_val_p))
    {
        LOG_ERR(CE_FILE_DISPATCH_e, "Null pointer");
        return false;
    }

    switch (type_e)
    {
        case TYPE_UINT8_e:
            return parse_u8(token_cp, &out_val_p->u8);

        case TYPE_UINT16_e:
            return parse_u16(token_cp, &out_val_p->u16);

        case TYPE_UINT32_e:
            return parse_u32(token_cp, &out_val_p->u32);

        case TYPE_UINT64_e:
            return parse_u64(token_cp, &out_val_p->u64);

        case TYPE_INT8_e:
            return parse_i8(token_cp, &out_val_p->i8);

        case TYPE_INT16_e:
            return parse_i16(token_cp, &out_val_p->i16);

        case TYPE_INT32_e:
            return parse_i32(token_cp, &out_val_p->i32);

        case TYPE_INT64_e:
            return parse_i64(token_cp, &out_val_p->i64);

        case TYPE_BOOL_e:
            return parse_boolean_token(token_cp, &out_val_p->b);

        case TYPE_STRING_e:
            out_val_p->str_cp = token_cp;
            return true;

        case TYPE_UINT8_PTR_e:
            if (NULL == scratch)
            {
                LOG_ERR(CE_FILE_DISPATCH_e, "Null scratch buffer");
                return false;
            }

            size_t bytes_len = 0;
            if (!parse_hex_string(token_cp, scratch->buffer + scratch->used, MAX_ARG_CONTENT_SIZE, &bytes_len))
            {
                LOG_ERR(CE_FILE_DISPATCH_e, "Failed to parse hex string: '%s'", token_cp);
                return false;
            }

            out_val_p->bytes_u8p = scratch->buffer + scratch->used;
            scratch->used += bytes_len;

            return true;

        default:
            LOG_ERR(CE_FILE_DISPATCH_e, "Unsupported argument type: %d", (int)type_e);
            return false;
    }
}

/**
 * @brief Parses string tokens into typed argument values (zero-copy, no malloc).
 *
 * This function uses the provided command signature to convert string tokens into
 * strongly typed values stored in `ce_arg_value_ut` entries. Scalars are parsed directly.
 * String arguments point into the pre-tokenized `line_buf`. Binary hex strings are parsed
 * into a shared static buffer and referenced by pointer.
 *
 * @param sig_pst        Pointer to the command's signature descriptor.
 * @param tokens_ca      Array of token pointers (command name + arguments).
 * @param out_args_a     Output array where parsed argument values will be stored.
 *
 * @return true if parsing succeeded for all arguments, false otherwise.
 *
 * @note
 * - Assumes `tokens_ca` was null-terminated and split in-place from `line_buf`.
 * - Caller must dispatch handler before the next `parse_arguments()` call.
 */

static bool parse_arguments(const ce_signature_st *sig_pst,
                            char *tokens_ca[],
                            ce_arg_value_ut out_args_a[MAX_TOKENS],
                            scratch_buffer_st *scratch)
{
    if ((NULL == sig_pst)    ||
        (NULL == tokens_ca)  ||
        (NULL == out_args_a) ||
        (NULL == scratch))
    {
        LOG_ERR(CE_FILE_DISPATCH_e, "Null pointer");
        return false;
    }

    if (sig_pst->type_count_u8 > MAX_PARSABLE_ARGUMENTS)
    {
        LOG_ERR(CE_FILE_DISPATCH_e, "Too many arguments: %u", sig_pst->type_count_u8);
        return false;
    }

    for (uint8_t i = 0u; i < sig_pst->type_count_u8; ++i)
    {
        const char *token_cp = tokens_ca[i + ARG_OFFSET];
        ce_arg_type_et type_e = sig_pst->types_e[i];

        if (NULL == token_cp)
        {
            LOG_ERR(CE_FILE_DISPATCH_e, "Missing argument %u", (unsigned)(i + 1u));
            return false;
        }

        if (!parse_value_by_type(type_e, token_cp, &out_args_a[i], scratch))
        {
            LOG_ERR(CE_FILE_DISPATCH_e,
                    "Failed to parse argument %u as type %d (token: '%s')",
                    (unsigned)(i + 1u), (int)type_e, token_cp);
            return false;
        }
    }

    return true;
}

/**
 * @brief Searches for a command signature by its hash.
 *
 * @param hash 32-bit hash of the command name.
 * @return Pointer to the signature if found, NULL otherwise.
 */
static const ce_signature_st *lookup_signature_by_hash(uint32_t hash)
{
    const ce_signature_st *table = ce_table_get_signatures();
    size_t count = ce_table_get_signatures_count();

    for (size_t i = 0; i < count; ++i)
    {
        if (table[i].hash_u32 == hash)
        {
            return &table[i];
        }
    }

    return NULL;
}

/**
 * @brief Splits a command line string into tokens in-place.
 *
 * This function modifies the input string by replacing whitespace with '\0' and
 * populates the tokens array with pointers to individual tokens.
 *
 * @param line Input string to tokenize (modified in-place).
 * @param tokens Output array to store pointers to tokens.
 * @param tokens_count Output parameter to receive number of tokens.
 *
 * @return true if tokenization succeeded without truncation, false otherwise.
 */
static bool parse_line_to_tokens(char *line, char *tokens[], size_t *tokens_count)
{
    size_t count = 0u;
    char *ptr = line;
    bool in_token = false;

    if ((NULL == line) || (NULL == tokens) || (NULL == tokens_count))
    {
        LOG_ERR(CE_FILE_DISPATCH_e, "Null pointer");
        return false;
    }

    while ((*ptr != '\0') && (count < MAX_TOKENS))
    {
        if (IS_WHITESPACE(*ptr))
        {
            *ptr = '\0';  /* split token */
            in_token = false;
        }
        else if (!in_token)
        {
            tokens[count++] = ptr;
            in_token = true;
        }
        else
        {
        }
        ptr++;
    }

    /* If we stopped due to reaching MAX_TOKENS but still inside a token,
     * consume the rest of this token to avoid partial token error
     */
    if (in_token && (count == MAX_TOKENS))
    {
        while ((*ptr != '\0') && !IS_WHITESPACE(*ptr))
        {
            ptr++;
        }
    }

    /* Now skip and null-terminate trailing whitespace */
    while ((*ptr != '\0') && IS_WHITESPACE(*ptr))
    {
        *ptr = '\0';
        ptr++;
    }

    if ((count == MAX_TOKENS) && (*ptr != '\0'))
    {
        LOG_ERR(CE_FILE_DISPATCH_e,
                "Too many tokens, maximum allowed is %u, input truncated at token count %u",
                (unsigned)MAX_TOKENS, (unsigned)count);
        return false;
    }

    *tokens_count = count;
    return true;
}

/**
 * @brief Validates and dispatches a command from tokenized input.
 *
 * @param sig_pst Pointer to the command signature.
 * @param tokens_ca Array of argument string tokens (NULL-terminated or fixed).
 *
 * @return true if execution succeeded, false otherwise.
 */
static bool dispatch_command(const ce_signature_st *sig_pst, char *tokens_ca[])
{
    ce_arg_value_ut args_a[MAX_TOKENS] = {0};
    scratch_buffer_st scratch = { .used = 0u };

    if ((NULL == sig_pst) || (NULL == tokens_ca))
    {
        LOG_ERR(CE_FILE_DISPATCH_e, "Null pointer");
        return false;
    }

    if (!parse_arguments(sig_pst, tokens_ca, args_a, &scratch))
    {
        LOG_ERR(CE_FILE_DISPATCH_e, "Failed to parse command arguments");
        return false;
    }

    if (!ce_invoke_handler(sig_pst, args_a))
    {
        return false;
    }

    return true;
}


/* Entry point of the command engine.
 * Parses a space-delimited input line and invokes the matching handler.
 */
bool ce_dispatch_from_line(const char *line_str)
{
    char  *tokens_apc[MAX_TOKENS] = {0};
    size_t tokens_count = 0u;
    uint32_t hash_u32;
    const ce_signature_st *sig_pst;
    char line_buf[MAX_LINE_BUF_SIZE] = {0};

    if (NULL == line_str)
    {
        LOG_ERR(CE_FILE_DISPATCH_e, "Null pointer");
        return false;
    }

    if (strlen(line_str) >= sizeof(line_buf))
    {
        LOG_ERR(CE_FILE_DISPATCH_e, "Input line truncated");
        return false;
    }

    strncpy(line_buf, line_str, sizeof(line_buf) - 1u);
    line_buf[sizeof(line_buf) - 1u] = '\0';

    if (!parse_line_to_tokens(line_buf, tokens_apc, &tokens_count))
    {
        return false;
    }

    if ((0u == tokens_count) || (NULL == tokens_apc[0]))
    {
        LOG_ERR(CE_FILE_DISPATCH_e, "No command token found");
        return false;
    }

    hash_u32 = ce_hash_calculate(tokens_apc[0]);
    sig_pst = lookup_signature_by_hash(hash_u32);

    if (NULL == sig_pst)
    {
        LOG_ERR(CE_FILE_DISPATCH_e, "Unknown command '%s' (hash=0x%08X)", tokens_apc[0], hash_u32);
        return false;
    }

    if (!validate_argument_count(tokens_count, sig_pst))
    {
        return false;
    }

    if (!dispatch_command(sig_pst, tokens_apc))
    {
        return false;
    }

    return true;
}


#ifdef UNIT_TEST

bool _test_validate_argument_count(int tokens_count, const ce_signature_st *sig_pst)
{
    return validate_argument_count(tokens_count, sig_pst);
}

bool _test_parse_arguments(const ce_signature_st *sig_pst,
                           char *tokens_ca[],
                           ce_arg_value_ut out_args_a[MAX_TOKENS],
                           scratch_buffer_st *scratch)
{
    return parse_arguments(sig_pst, tokens_ca, out_args_a, scratch);
}

bool _test_parse_line_to_tokens(char *line, char *tokens[], size_t *tokens_count)
{
    return parse_line_to_tokens(line, tokens, tokens_count);
}

bool _test_dispatch_command(const ce_signature_st *sig_pst, char *tokens_ca[])
{
    dispatch_command(sig_pst, tokens_ca);
}

const ce_signature_st* _test_lookup_signature_by_hash(uint32_t hash)
{
    return lookup_signature_by_hash(hash);
}

#endif /* UNIT_TEST */
