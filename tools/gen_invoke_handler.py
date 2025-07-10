# SPDX-License-Identifier: Apache-2.0
"""
Invoke handler generator from YAML command definitions.
"""

from pathlib import Path
from utils import TYPE_MAP


class InvokeGenerator:
    """
    Generates the `invoke_handler()` function as a C source file.

    This function performs runtime dispatch of command handlers
    by switching on the command hash and invoking the handler
    with the properly casted arguments.

    It relies on `ARGVAL(type, value)` to extract argument values
    and cast them to their correct types for the handler call.
    """

    def __init__(self, commands, includes):
        """
        Initialize the generator.

        Args:
            commands (list): List of command dictionaries parsed from YAML.
            includes (list): List of additional header files to include.
        """
        self.commands = commands
        self.includes = includes

    def render(self, filename: str) -> str:
        """
        Render the full C source for the invoke_handler file.

        Args:
            filename (str): File name for documentation comments.

        Returns:
            str: Full contents of the generated C source.
        """
        lines = [
            "/* SPDX-License-Identifier: Apache-2.0 */",
            "/**",
            f" * @file {Path(filename).name}",
            " * @brief Auto-generated command invocation handler from YAML definitions.",
            " *",
            " * This source file implements the `invoke_handler()` function, which",
            " * dispatches commands to their respective handler functions based on",
            " * command hash values and associated argument type information.",
            " *",
            " * @note This file is auto-generated. Do not modify manually as changes",
            " *       will be overwritten by the code generator.",
            " */",
            "",
            "#include <stdint.h>",
            "#include <stdio.h>",
            '#include "ce_types.h"',
            '#include "ce_command_ids.h"',
            "",
        ]

        # Add any additional includes from YAML
        for inc in self.includes:
            lines.append(f'#include "{inc}"')

        lines += [
            "",
            "bool ce_invoke_handler(const ce_signature_st *sig_pst,",
            "                       const ce_arg_value_ut args_a[MAX_TOKENS])",
            "{",
            "    switch ((ce_cmd_hash_et)sig_pst->hash_u32)",
            "    {",
        ]

        for cmd in self.commands:
            name = cmd["name"]
            args = cmd.get("args", [])

            # Comment with the command name
            lines.append(f"        case CE_CMD_{name.upper()}_e:")

            # Determine C types from argument types
            cast_types = [TYPE_MAP[arg["type"]][1] for arg in args]

            # Generate the casted function pointer
            if not cast_types:
                # No args
                lines.append("            return ((bool (*)(void))sig_pst->handler)();")
            else:
                # Signature: bool (*)(type1, type2, ...)
                sig_cast = f"bool (*)({', '.join(cast_types)})"

                lines.append(f"            return (({sig_cast})sig_pst->handler)(")

                # Cast and extract each argument
                arg_lines = []
                for i, typ in enumerate(cast_types):
                    val = f"ARGVAL(sig_pst->types_e[{i}], args_a[{i}])"
                    if typ in ["const char *", "const uint8_t *"]:
                        arg_lines.append(f"                ({typ})(uintptr_t){val}")
                    else:
                        arg_lines.append(f"                ({typ}){val}")

                # Join all arguments
                lines.append(",\n".join(arg_lines) + "\n            );")

        # Default case for unknown commands
        lines += [
            "        default:",
            "            return false;",
            "    }",
            "}",
            "",
        ]

        return "\n".join(lines)

    def write_to(self, path: Path):
        """
        Write the generated C source to a file.

        Args:
            path (Path): Full path to the output .c file.
        """
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(self.render(path.name), encoding="utf-8")
