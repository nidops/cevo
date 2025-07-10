# SPDX-License-Identifier: Apache-2.0
"""
Header generator from YAML command definitions.
"""

from pathlib import Path
from utils import djb2_hash


class HeaderGenerator:
    """
    Generate the output C header file defining command hash macros.
    """

    def __init__(self, commands):
        self.commands = commands

    def render(self, filename: str) -> str:
        """
        Produce the contents of the header as a string.

        Args:
            filename: File name used for include guard.

        Returns:
            A string representing the header content.
        """
        guard = Path(filename).stem.upper() + "_H"
        lines = [
            "/* SPDX-License-Identifier: Apache-2.0 */",
            "/**",
            f" * @file {Path(filename).name}",
            " * @brief Auto-generated command ID definitions based on YAML configuration.",
            " *",
            " * This header defines unique 32-bit hashes for each command supported by the",
            " * Command Engine. These hashes are used internally for efficient command",
            " * identification and dispatch.",
            " *",
            " * @note This file is auto-generated. Do not modify manually as changes",
            " *       will be overwritten by the code generator.",
            " */",
            "",
            f"#ifndef {guard}",
            f"#define {guard}",
            "",
        ]

        lines += self._prepare_enum()
        lines.append(f"#endif /* {guard} */\n")
        return "\n".join(lines)

    def _prepare_enum(self):
        """
        Generate a strongly-typed enum of command hashes.

        Returns:
            A list of strings representing enum lines.
        """
        enum_lines = ["typedef enum {"]
        for cmd in self.commands:
            enum_name = f"CE_CMD_{cmd['name'].upper()}_e"
            value = djb2_hash(cmd["name"])
            enum_lines.append(f"    {enum_name:<32} = 0x{value:08X}u,")
        enum_lines.append("} ce_cmd_hash_et;\n")
        return enum_lines

    def _prepare_macros(self):
        """
        [DEPRECATED] Generate legacy hash macros.

        This function is retained for reference or fallback compatibility,
        but not used in current output (enums are preferred).
        """
        lines = []
        for cmd in self.commands:
            macro = f"CE_HASH_CMD_{cmd['name'].upper()}"
            value = djb2_hash(cmd["name"])
            lines.append(f"#define {macro:<32} 0x{value:08X}u")
        return lines

    def write_to(self, path: Path):
        """Write the header content to the specified path."""
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(self.render(path.name), encoding="utf-8")
