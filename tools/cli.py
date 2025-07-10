# SPDX-License-Identifier: Apache-2.0
"""
Command-line argument parser for the YAML-to-C generator.
"""

import argparse
from pathlib import Path

VERSION = "v1.0"


class CliArgs:
    """
    Parses command-line arguments for the YAML-to-C command engine generator.

    Supports generation of:
      - Header file with hash defines
      - Dispatch table source
      - Runtime invoke handler

    Also supports dry-run, validation-only, and verbose output modes.
    """

    def __init__(self):
        self.parser = argparse.ArgumentParser(
            description="Generate C header and source files from a YAML command definition."
        )

        # Required YAML input file
        self.parser.add_argument(
            "-i", "--input", type=Path, required=True, help="Path to input YAML definition file"
        )

        # Optional output targets
        self.parser.add_argument(
            "--header",
            default="ce_command_ids.h",
            help="Output header file for command hashes (default: ce_command_ids.h)",
        )
        self.parser.add_argument(
            "--sigtable",
            default="ce_table.c",
            help="Output C source file for dispatch table (default: ce_table.c)",
        )
        self.parser.add_argument(
            "--invoke",
            default="ce_invoke_handler.c",
            help="Output C source file for invoke handler (default: ce_invoke_handler.c)",
        )

        # Flags
        self.parser.add_argument(
            "--dry-run",
            action="store_true",
            help="Print generated files to stdout instead of writing",
        )
        self.parser.add_argument(
            "--check-only",
            action="store_true",
            help="Validate YAML input only, no file output",
        )
        self.parser.add_argument(
            "--verbose",
            action="store_true",
            help="Print generation progress and details",
        )
        self.parser.add_argument(
            "--version", action="version", version=f"generate_apis.py {VERSION}"
        )

    def parse(self):
        """
        Parse and return the command-line arguments.

        @return: argparse.Namespace with parsed arguments
        """
        return self.parser.parse_args()
