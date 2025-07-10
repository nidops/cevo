# SPDX-License-Identifier: Apache-2.0
"""
Source code generators from YAML command definitions.
"""

from pathlib import Path
from utils import djb2_hash, TYPE_MAP


class SignatureTableGenerator:
    """
    Generate the output C source file for the dispatch table.
    """

    def __init__(self, commands, includes):
        self.commands = commands
        self.includes = includes

    def render(self, filename: str) -> str:
        """
        Produce the contents of the source as a string.

        Args:
            filename: File name for documentation purposes.

        Returns:
            The complete C source code as a single string.
        """
        lines = [
            "/* SPDX-License-Identifier: Apache-2.0 */",
            "/**",
            f" * @file {Path(filename).name}",
            " * @brief Auto-generated dispatch table for command handlers.",
            " *",
            " * This file contains the command signature table generated from the",
            " * YAML configuration. It defines command hashes, their associated",
            " * handler functions, and argument type signatures.",
            " *",
            " * It also provides accessor functions to retrieve the command signatures",
            " * at runtime.",
            " *",
            " * @note This file is auto-generated. Do not modify manually as changes",
            " *       will be overwritten by the code generator.",
            " */",
            "",
            "#include <stddef.h>",
            "#include <stdint.h>",
            "#include <stdbool.h>",
            '#include "ce_table.h"',
            '#include "ce_command_ids.h"',
            "",
        ]
        lines.extend([f'#include "{inc}"' for inc in self.includes])
        lines += [
            "",
            "/* Compile-time type checking for function signatures */",
            "#if defined(__GNUC__) || defined(__clang__)",
            "#define CE_ASSERT_ARGS(fn, ...)                  \\",
            "    _Static_assert(__builtin_types_compatible_p( \\",
            "        typeof(&(fn)), bool (*)(__VA_ARGS__)     \\",
            '    ), "Signature mismatch: " #fn)',
            "#else",
            '#warning "CE_ASSERT_ARGS: function signature checks not available on this compiler!"',
            "#define CE_ASSERT_ARGS(fn, ...)",
            "#endif",
            "",
        ]

        for cmd in self.commands:
            types = [TYPE_MAP[arg["type"]][1] for arg in cmd.get("args", [])]
            lines.append(f"CE_ASSERT_ARGS({cmd['handler']}, {', '.join(types) or 'void'});")

        lines.append("")

        for cmd in self.commands:
            name = cmd["name"]
            enums = [TYPE_MAP[arg["type"]][0] for arg in cmd.get("args", [])]
            lines.append(f"static const ce_arg_type_et ce_args_{name}_ae[] = {{")
            lines.append(f"    {', '.join(enums)}") if enums else None
            lines.append("};\n")

        lines.append("static const ce_signature_st ce_signature_table_ast[] = {")
        for cmd in self.commands:
            lines.append("    {")
            lines.append(f"        .hash_u32 = 0x{djb2_hash(cmd['name']):08X}u,")
            lines.append(f"        .handler = (handler_func_t){cmd['handler']},")
            lines.append(f"        .types_e = ce_args_{cmd['name']}_ae,")
            lines.append(f"        .type_count_u8 = {len(cmd.get('args', []))}u")
            lines.append("    },")
        lines.append("};\n")

        lines += [
            "const ce_signature_st* ce_table_get_signatures(void)",
            "{",
            "    return ce_signature_table_ast;",
            "}\n",
            "size_t ce_table_get_signatures_count(void)",
            "{",
            "    return sizeof(ce_signature_table_ast) / sizeof(ce_signature_table_ast[0]);",
            "}\n",
        ]
        return "\n".join(lines)

    def write_to(self, path: Path):
        """Write the source content to the specified path."""
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(self.render(path.name), encoding="utf-8")
