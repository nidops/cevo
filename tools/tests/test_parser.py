# SPDX-License-Identifier: Apache-2.0
"""
Unit tests for YAML validation in parser.py
"""

import pytest
from pathlib import Path
from parser import CommandParser


# --------------------------------------------------------------------
# Shared Helper
# --------------------------------------------------------------------
def write_yaml(tmp_path, content: str) -> Path:
    """Write YAML content to a temp file and return the path."""
    path = tmp_path / "test.yaml"
    path.write_text(content)
    return path


# --------------------------------------------------------------------
# Positive Case
# --------------------------------------------------------------------
def test_parser_valid(tmp_path):
    """A well-formed YAML should be accepted and parsed correctly."""
    yaml = """
    commands:
      - name: test_cmd
        handler: test_func
        args:
          - type: uint8
    """
    file = write_yaml(tmp_path, yaml)
    parser = CommandParser(file)
    parser.load()
    assert len(parser.commands) == 1
    assert parser.commands[0]["name"] == "test_cmd"


# --------------------------------------------------------------------
# Structural Errors
# --------------------------------------------------------------------
def test_parser_missing_name(tmp_path):
    """Missing 'name' should raise an error."""
    yaml = """
    commands:
      - handler: test_func
        args:
          - type: uint8
    """
    file = write_yaml(tmp_path, yaml)
    parser = CommandParser(file)
    with pytest.raises(ValueError, match="missing 'name' or 'handler'"):
        parser.load()


def test_parser_missing_handler(tmp_path):
    """Missing 'handler' should raise an error."""
    yaml = """
    commands:
      - name: my_cmd
        args:
          - type: uint8
    """
    file = write_yaml(tmp_path, yaml)
    parser = CommandParser(file)
    with pytest.raises(ValueError, match="missing 'name' or 'handler'"):
        parser.load()


# --------------------------------------------------------------------
# Semantic Errors
# --------------------------------------------------------------------
def test_parser_invalid_type(tmp_path):
    """Unsupported argument type should raise an error."""
    yaml = """
    commands:
      - name: bad_type
        handler: bad_func
        args:
          - type: float64
    """
    file = write_yaml(tmp_path, yaml)
    parser = CommandParser(file)
    with pytest.raises(ValueError, match="Unsupported type 'float64'"):
        parser.load()


def test_parser_invalid_name_format(tmp_path):
    """Invalid C identifier as command name should be rejected."""
    yaml = """
    commands:
      - name: "123-invalid"
        handler: valid_handler
        args: []
    """
    file = write_yaml(tmp_path, yaml)
    parser = CommandParser(file)
    with pytest.raises(ValueError, match=r"Invalid command name '123-invalid'"):
        parser.load()


def test_parser_invalid_handler_format(tmp_path):
    """Invalid C identifier as handler name should be rejected."""
    yaml = """
    commands:
      - name: valid_name
        handler: "invalid handler"
        args: []
    """
    file = write_yaml(tmp_path, yaml)
    parser = CommandParser(file)
    with pytest.raises(ValueError, match=r"Invalid handler name 'invalid handler'"):
        parser.load()


def test_parser_duplicate_names(tmp_path):
    """Duplicate command names should be rejected before hashing."""
    yaml = """
    commands:
      - name: ping
        handler: h1
      - name: ping
        handler: h2
    """
    file = write_yaml(tmp_path, yaml)
    parser = CommandParser(file)
    with pytest.raises(ValueError, match=r"Duplicate command name: 'ping'"):
        parser.load()


def test_parser_field_order_flexibility(tmp_path):
    """Command with out-of-order fields should still parse correctly."""
    yaml = """
    commands:
      - handler: do_stuff
        args:
          - type: uint8
        name: shuffle
    """
    file = write_yaml(tmp_path, yaml)
    parser = CommandParser(file)
    parser.load()
    assert parser.commands[0]["name"] == "shuffle"
