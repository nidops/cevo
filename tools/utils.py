# SPDX-License-Identifier: Apache-2.0
"""
Utility definitions for hash function and type mapping.
"""

# Static map: YAML string types -> (C enum, C type)
# Aliases provided for user flexibility (e.g., u8 == uint8)
TYPE_MAP = {
    # Unsigned integers
    "uint8": ("TYPE_UINT8_e", "uint8_t"),
    "uint16": ("TYPE_UINT16_e", "uint16_t"),
    "uint32": ("TYPE_UINT32_e", "uint32_t"),
    "uint64": ("TYPE_UINT64_e", "uint64_t"),
    "u8": ("TYPE_UINT8_e", "uint8_t"),
    "u16": ("TYPE_UINT16_e", "uint16_t"),
    "u32": ("TYPE_UINT32_e", "uint32_t"),
    "u64": ("TYPE_UINT64_e", "uint64_t"),
    # Signed integers
    "int8": ("TYPE_INT8_e", "int8_t"),
    "int16": ("TYPE_INT16_e", "int16_t"),
    "int32": ("TYPE_INT32_e", "int32_t"),
    "int64": ("TYPE_INT64_e", "int64_t"),
    "i8": ("TYPE_INT8_e", "int8_t"),
    "i16": ("TYPE_INT16_e", "int16_t"),
    "i32": ("TYPE_INT32_e", "int32_t"),
    "i64": ("TYPE_INT64_e", "int64_t"),
    # Native types
    "bool": ("TYPE_BOOL_e", "bool"),
    "b": ("TYPE_BOOL_e", "bool"),
    # String
    "string": ("TYPE_STRING_e", "const char*"),
    "str": ("TYPE_STRING_e", "const char*"),
    "s": ("TYPE_STRING_e", "const char*"),
    # Binary buffer (parsed hex)
    "uint8_ptr": ("TYPE_UINT8_PTR_e", "const uint8_t*"),
    "u8p": ("TYPE_UINT8_PTR_e", "const uint8_t*"),
}


def djb2_hash(string: str) -> int:
    """Compute a 32-bit DJB2 hash."""
    hash_value = 5381
    for char in string:
        hash_value = ((hash_value << 5) + hash_value) + ord(char)
    return hash_value & 0xFFFFFFFF
