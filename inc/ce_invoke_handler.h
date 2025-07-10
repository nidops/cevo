/* SPDX-License-Identifier: Apache-2.0 */
/**
 * @file ce_invoke_handler.h
 * @brief Public interface for invoking command handlers at runtime.
 *
 * This module provides the runtime dispatch entry point that dynamically
 * calls registered command handlers using parsed arguments.
 */

#ifndef CE_INVOKE_HANDLER_H
#define CE_INVOKE_HANDLER_H

#include "ce_types.h"  // For ce_signature_st and ce_arg_value_ut

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Invoke a command handler using its signature and parsed arguments.
 *
 * This function dispatches a command handler using the provided metadata and
 * parsed argument values. It assumes that:
 * - `sig_pst` is non-null and contains a valid function pointer.
 * - `args_a` has at least `sig_pst->type_count_u8` entries.
 * - All entries in `args_a` match the corresponding types in `sig_pst->types_e[]`.
 *
 * No argument type checking is performed. Callers must validate before dispatch.
 *
 * @param sig_pst Pointer to the signature of the handler to invoke.
 * @param args_a  Array of argument values (must match signature exactly).
 *
 * @return true if the handler executed successfully, false otherwise.
 */
bool ce_invoke_handler(const ce_signature_st *sig_pst,
                       const ce_arg_value_ut args_a[]);

#ifdef __cplusplus
}
#endif

#endif /* CE_INVOKE_HANDLER_H */
