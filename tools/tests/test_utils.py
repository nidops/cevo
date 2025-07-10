# SPDX-License-Identifier: Apache-2.0
"""
Unit tests for hash function in utils.py
"""

import sys
import os

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))

from utils import djb2_hash


def test_djb2_hash_known_values():
    """
    Ensure djb2_hash returns consistent, correct values for known strings.
    These are stable results manually verified.
    """
    assert djb2_hash("set_mac") == 0x8C2AFEA1
    assert djb2_hash("set_speed") == 0x435A0D81
    assert djb2_hash("reset") == 0x10474288


def test_djb2_hash_empty_string():
    """Hash of empty string should be the DJB2 initial value (5381)."""
    assert djb2_hash("") == 5381


def test_djb2_hash_different_strings():
    """Different strings should produce different hashes."""
    assert djb2_hash("command") != djb2_hash("cmomand")
    assert djb2_hash("command") != djb2_hash("commadn")


def test_djb2_hash_case_sensitive():
    """Hash should be case sensitive."""
    assert djb2_hash("Command") != djb2_hash("command")
    assert djb2_hash("command") != djb2_hash("commAnd")


def test_djb2_hash_unicode():
    """
    Hash should handle Unicode strings (assuming utf-8 encoding in implementation).
    Just verify it returns an integer.
    """
    result = djb2_hash("cömmand")
    assert isinstance(result, int)
