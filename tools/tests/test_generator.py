# SPDX-License-Identifier: Apache-2.0
"""
Unit tests for C code generation.
"""

import sys
import os
import pytest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))

from gen_header import HeaderGenerator
from gen_signature_table import SignatureTableGenerator
from gen_invoke_handler import InvokeGenerator
from utils import djb2_hash


sample_cmds = [
    {"name": "reset", "handler": "reset_handler"},
    {"name": "ping", "handler": "ping_handler", "args": [{"type": "uint8"}]},
    {
        "name": "transmit",
        "handler": "tx_handler",
        "args": [{"type": "uint8_ptr"}, {"type": "uint8"}],
    },
    {"name": "echo", "handler": "echo_handler", "args": [{"type": "str"}]},
]


def test_header_macro_output():
    """
    Check that the header generator defines the correct macro.
    """
    gen = HeaderGenerator(sample_cmds)
    content = gen.render("core_api.h")
    assert "CE_CMD_PING_e" in content
    assert "core_api.h" in content


def test_signature_table_output():
    """
    Verify that the signature table renders correctly.
    """
    gen = SignatureTableGenerator(sample_cmds, [])
    src = gen.render("ce_table.c")
    assert "ping_handler" in src
    assert "ce_args_ping_ae" in src
    assert "CE_ASSERT_ARGS" in src


def test_invoke_handler_output():
    """
    Check the invoke handler rendering includes expected dispatch logic.
    """
    gen = InvokeGenerator(sample_cmds, [])
    code = gen.render("ce_invoke_handler.c")
    assert "bool ce_invoke_handler" in code
    assert "case CE_CMD_" in code  # switch-case based on hash
    assert "sig_pst->handler" in code
    assert "uint8_t" in code  # casted argument type


@pytest.mark.parametrize(
    "cmds",
    [
        # No-arg command
        [{"name": "reset", "handler": "reset_handler"}],
        # Single uint8 arg
        [{"name": "ping", "handler": "ping_handler", "args": [{"type": "uint8"}]}],
        # Mixed args
        [
            {
                "name": "transmit",
                "handler": "tx_handler",
                "args": [{"type": "uint8_ptr"}, {"type": "uint8"}],
            }
        ],
        # String alias
        [{"name": "echo", "handler": "echo_handler", "args": [{"type": "str"}]}],
    ],
)
def test_generators_render(cmds):
    """
    Validate rendering of header, signature table, and invoke handler
    for various command configurations.
    """
    header_gen = HeaderGenerator(cmds)
    sigtable_gen = SignatureTableGenerator(cmds, [])
    invoke_gen = InvokeGenerator(cmds, [])

    header = header_gen.render("api.h")
    table = sigtable_gen.render("ce_table.c")
    invoke = invoke_gen.render("ce_invoke_handler.c")

    # Basic checks
    for cmd in cmds:
        name = cmd["name"].upper()
        assert f"CE_CMD_{name}" in header

        assert cmd["handler"] in table
        assert f"ce_args_{cmd['name']}_ae" in table

        assert f"case CE_CMD_{cmd['name'].upper()}_e" in invoke
        assert "invoke_handler" in invoke
        for arg in cmd.get("args", []):
            ctype = arg["type"]
            if ctype in ["uint8", "u8"]:
                assert "uint8_t" in invoke
            elif ctype in ["uint16", "u16"]:
                assert "uint16_t" in invoke
            elif ctype in ["string", "str"]:
                assert "(const char*)" in invoke
            elif ctype in ["uint8_ptr", "u8p"]:
                assert "(const uint8_t*)" in invoke


def test_enum_hash_values():
    """
    Verify that the enum contains the correct names and hash values.
    """
    gen = HeaderGenerator(sample_cmds)
    content = gen.render("core_api.h")

    expected = {
        "RESET": djb2_hash("reset"),
        "PING": djb2_hash("ping"),
        "TRANSMIT": djb2_hash("transmit"),
        "ECHO": djb2_hash("echo"),
    }

    for name, value in expected.items():
        enum_line = f"CE_CMD_{name}_e"
        value_str = f"= 0x{value:08X}u"
        assert enum_line in content
        assert value_str in content
