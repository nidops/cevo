#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
"""
@file generate_apis.py
@brief Command-line entry point for YAML-to-C code generation.

This script serves as the CLI interface for generating C source files
from a YAML command specification. It delegates all logic to `main.py`
and is typically invoked by build systems such as CMake.

Generated outputs include:
  - `ce_command_ids.h`     : Command hash defines (used in host/MCU)
  - `ce_table.c`           : Signature table linking hashes to handlers
  - `ce_invoke_handler.c`  : Generic runtime dispatcher

Usage:
    python tools/generate_apis.py input.yaml [--header out.h] [--sigtable out.c] [--invoke out.c] [options]

Options:
    --header        Output header file for command hashes (default: ce_command_ids.h)
    --sigtable      Output C source file for dispatch table (default: ce_table.c)
    --invoke        Output C source file for invoke handler (default: apis/ce_invoke_handler.c)
    --check-only    Validate YAML structure only, no code is generated
    --dry-run       Print generated code to stdout without writing to files
    --verbose       Print detailed info during parsing and code generation
"""

from main import main

if __name__ == "__main__":
    main()
