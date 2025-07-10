# SPDX-License-Identifier: Apache-2.0
"""
Main entry point for YAML-to-C command generator.
"""

import sys
from pathlib import Path
from parser import CommandParser
from gen_header import HeaderGenerator
from gen_signature_table import SignatureTableGenerator
from gen_invoke_handler import InvokeGenerator
from cli import CliArgs


def main():
    """
    Load YAML input, validate content, and emit C files (header + sources).
    Supports dry-run, check-only, and verbose modes.
    """
    args = CliArgs().parse()
    try:
        if not args.input:
            raise ValueError("Missing required argument: --input")

        parser = CommandParser(Path(args.input))
        parser.load()

        if args.check_only:
            print(f"✅ YAML '{args.input}' is valid with {len(parser.commands)} command(s).")
            sys.exit(0)

        # === Code Generators ===
        header_gen = HeaderGenerator(parser.commands)
        table_gen = SignatureTableGenerator(parser.commands, parser.includes)
        invoke_gen = InvokeGenerator(parser.commands, parser.includes)

        if args.dry_run:
            print("\n// ==== HEADER FILE ====\n")
            print(header_gen.render(args.header))

            print("\n// ==== SIGNATURE TABLE ====\n")
            print(table_gen.render(args.sigtable))

            print("\n// ==== INVOKE HANDLER ====\n")
            print(invoke_gen.render(args.invoke))
        else:
            header_gen.write_to(Path(args.header))
            table_gen.write_to(Path(args.sigtable))
            invoke_gen.write_to(Path(args.invoke))

            if args.verbose:
                print("✅ Generated:")
                print(f"  - {args.header}")
                print(f"  - {args.sigtable}")
                print(f"  - {args.invoke}")
                print("📦 Commands:")
                for cmd in parser.commands:
                    print(f"  - {cmd['name']}")

    except Exception as e:
        print(f"❌ Error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
