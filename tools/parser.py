# SPDX-License-Identifier: Apache-2.0
"""
YAML command definition parser and validator.
"""

from pathlib import Path
from ruamel.yaml import YAML
from utils import djb2_hash, TYPE_MAP


class CommandParser:
    """
    Load and validate the YAML input defining commands and includes.
    Ensures structural correctness and checks for invalid or duplicate hash usage.
    """

    ALLOWED_KEYS = {"commands", "includes"}

    def __init__(self, input_file: Path):
        self.yaml = YAML()
        self.yaml.preserve_quotes = True
        self.yaml.indent(mapping=2, sequence=4, offset=2)
        self.input_file = input_file
        self.commands = []
        self.includes = []

    def load(self):
        """Load and parse the YAML file, then validate its structure."""
        if not self.input_file.exists():
            raise FileNotFoundError(f"YAML input file not found: {self.input_file}")
        data = self.yaml.load(self.input_file)

        if data is None:
            raise ValueError(f"YAML input file '{self.input_file}' is empty or invalid")

        # Ensure only known top-level keys are present
        extra = set(data.keys()) - self.ALLOWED_KEYS
        if extra:
            raise ValueError(f"Unsupported YAML section(s): {', '.join(extra)}")

        self.commands = data.get("commands", [])
        self.includes = data.get("includes", [])

        if not isinstance(self.commands, list):
            raise ValueError("'commands' must be a list")
        if not isinstance(self.includes, list):
            raise ValueError("'includes' must be a list")

        self._validate()

    def _validate(self):
        """Validate command structure, types, name formats, and detect hash collisions."""
        seen_hashes = {}
        seen_names = set()

        for i, cmd in enumerate(self.commands):
            name = cmd.get("name")
            handler = cmd.get("handler")

            if not name or not handler:
                raise ValueError(f"Command #{i + 1} is missing 'name' or 'handler'")

            if not self._is_valid_c_identifier(name):
                raise ValueError(f"❌ Invalid command name '{name}': must be a valid C identifier")

            if not self._is_valid_c_identifier(handler):
                raise ValueError(
                    f"❌ Invalid handler name '{handler}': must be a valid C identifier"
                )

            if name in seen_names:
                raise ValueError(f"❌ Duplicate command name: '{name}'")
            seen_names.add(name)

            hash_val = djb2_hash(name)
            if hash_val in seen_hashes:
                raise ValueError(
                    f"❌ Hash collision: '{name}' and '{seen_hashes[hash_val]}' both hash to 0x{hash_val:08X}"
                )
            seen_hashes[hash_val] = name

            args = cmd.get("args", [])
            if not isinstance(args, list):
                raise ValueError(f"'args' for command '{name}' must be a list")

            for arg in args:
                if not isinstance(arg, dict):
                    raise ValueError(f"❌ Each argument of command '{name}' must be a dict")
                if "type" not in arg:
                    raise ValueError(f"❌ Missing 'type' in command '{name}' argument")
                if arg["type"] not in TYPE_MAP:
                    raise ValueError(f"❌ Unsupported type '{arg['type']}' in command '{name}'")

    @staticmethod
    def _is_valid_c_identifier(identifier: str) -> bool:
        """Return True if the given name is a valid C identifier."""
        if not identifier:
            return False
        if not (identifier[0].isalpha() or identifier[0] == "_"):
            return False
        return all(c.isalnum() or c == "_" for c in identifier)
