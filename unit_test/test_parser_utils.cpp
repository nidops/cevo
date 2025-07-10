/* SPDX-License-Identifier: Apache-2.0 */

#include "gtest/gtest.h"
#include "parser_utils.h"

//----------------------------------------------------------------------------//
// parse_u8 tests
//----------------------------------------------------------------------------//

TEST(ParseU8Test, ValidInputs) {
    uint8_t out;

    EXPECT_TRUE(parse_u8("0", &out)); EXPECT_EQ(out, 0u);
    EXPECT_TRUE(parse_u8("255", &out)); EXPECT_EQ(out, 255u);
    EXPECT_TRUE(parse_u8("001", &out)); EXPECT_EQ(out, 1u);
}

TEST(ParseU8Test, InvalidInputs) {
    uint8_t out;

    EXPECT_FALSE(parse_u8("256", &out));
    EXPECT_FALSE(parse_u8("abc", &out));
    EXPECT_FALSE(parse_u8(NULL, &out));
    EXPECT_FALSE(parse_u8("123", NULL));
    EXPECT_FALSE(parse_u8("", &out));
}

//----------------------------------------------------------------------------//
// parse_i8 tests
//----------------------------------------------------------------------------//

TEST(ParseI8Test, ValidInputs) {
    int8_t out;

    EXPECT_TRUE(parse_i8("0", &out)); EXPECT_EQ(out, 0);
    EXPECT_TRUE(parse_i8("-128", &out)); EXPECT_EQ(out, -128);
    EXPECT_TRUE(parse_i8("127", &out)); EXPECT_EQ(out, 127);
    EXPECT_TRUE(parse_i8("+42", &out)); EXPECT_EQ(out, 42);
}

TEST(ParseI8Test, InvalidInputs) {
    int8_t out;

    EXPECT_FALSE(parse_i8("128", &out));
    EXPECT_FALSE(parse_i8("-129", &out));
    EXPECT_FALSE(parse_i8("abc", &out));
    EXPECT_FALSE(parse_i8(NULL, &out));
    EXPECT_FALSE(parse_i8("123", NULL));
    EXPECT_FALSE(parse_i8("", &out));
}

//----------------------------------------------------------------------------//
// parse_u16 tests
//----------------------------------------------------------------------------//

TEST(ParseU16Test, ValidInputs) {
    uint16_t out;

    EXPECT_TRUE(parse_u16("0", &out)); EXPECT_EQ(out, 0u);
    EXPECT_TRUE(parse_u16("65535", &out)); // UINT16_MAX
    EXPECT_EQ(out, UINT16_MAX);
    EXPECT_TRUE(parse_u16("0001", &out)); EXPECT_EQ(out, 1u);
    EXPECT_TRUE(parse_u16("42", &out)); EXPECT_EQ(out, 42u);
}

TEST(ParseU16Test, InvalidInputs) {
    uint16_t out;

    // Overflow: UINT16_MAX + 1 (65536)
    EXPECT_FALSE(parse_u16("65536", &out));

    EXPECT_FALSE(parse_u16("abc", &out));
    EXPECT_FALSE(parse_u16(NULL, &out));
    EXPECT_FALSE(parse_u16("", &out));
    EXPECT_FALSE(parse_u16("655350", &out)); // extra digit
    EXPECT_FALSE(parse_u16("42", NULL));
}

//----------------------------------------------------------------------------//
// parse_i16 tests
//----------------------------------------------------------------------------//

TEST(ParseI16Test, ValidInputs) {
    int16_t out;

    EXPECT_TRUE(parse_i16("0", &out)); EXPECT_EQ(out, 0);
    EXPECT_TRUE(parse_i16("-32768", &out)); // INT16_MIN
    EXPECT_EQ(out, INT16_MIN);
    EXPECT_TRUE(parse_i16("32767", &out));  // INT16_MAX
    EXPECT_EQ(out, INT16_MAX);
    EXPECT_TRUE(parse_i16("-1", &out)); EXPECT_EQ(out, -1);
    EXPECT_TRUE(parse_i16("+42", &out)); EXPECT_EQ(out, 42);
}

TEST(ParseI16Test, InvalidInputs) {
    int16_t out;

    // Overflow cases:
    EXPECT_FALSE(parse_i16("32768", &out));  // INT16_MAX + 1
    EXPECT_FALSE(parse_i16("-32769", &out)); // INT16_MIN - 1

    EXPECT_FALSE(parse_i16("abc", &out));
    EXPECT_FALSE(parse_i16(NULL, &out));
    EXPECT_FALSE(parse_i16("", &out));
    EXPECT_FALSE(parse_i16("42", NULL));
}

//----------------------------------------------------------------------------//
// parse_u32 tests
//----------------------------------------------------------------------------//

TEST(ParseU32Test, ValidInputs) {
    uint32_t out;

    EXPECT_TRUE(parse_u32("0", &out)); EXPECT_EQ(out, 0u);
    EXPECT_TRUE(parse_u32("4294967295", &out)); // UINT32_MAX
    EXPECT_EQ(out, UINT32_MAX);
    EXPECT_TRUE(parse_u32("0001", &out)); EXPECT_EQ(out, 1u);
    EXPECT_TRUE(parse_u32("42", &out)); EXPECT_EQ(out, 42u);
}

TEST(ParseU32Test, InvalidInputs) {
    uint32_t out;

    // Overflow: UINT32_MAX + 1 (4294967296)
    EXPECT_FALSE(parse_u32("4294967296", &out));

    EXPECT_FALSE(parse_u32("abc", &out));
    EXPECT_FALSE(parse_u32(NULL, &out));
    EXPECT_FALSE(parse_u32("", &out));
    EXPECT_FALSE(parse_u32("42949672950", &out)); // extra digit
    EXPECT_FALSE(parse_u32("42", NULL));
}

//----------------------------------------------------------------------------//
// parse_i32 tests
//----------------------------------------------------------------------------//

TEST(ParseI32Test, ValidInputs) {
    int32_t out;

    EXPECT_TRUE(parse_i32("0", &out)); EXPECT_EQ(out, 0);
    EXPECT_TRUE(parse_i32("-2147483648", &out)); // INT32_MIN
    EXPECT_EQ(out, INT32_MIN);
    EXPECT_TRUE(parse_i32("2147483647", &out));  // INT32_MAX
    EXPECT_EQ(out, INT32_MAX);
    EXPECT_TRUE(parse_i32("-1", &out)); EXPECT_EQ(out, -1);
    EXPECT_TRUE(parse_i32("+42", &out)); EXPECT_EQ(out, 42);
}

TEST(ParseI32Test, InvalidInputs) {
    int32_t out;

    // Overflow cases:
    EXPECT_FALSE(parse_i32("2147483648", &out));  // INT32_MAX + 1
    EXPECT_FALSE(parse_i32("-2147483649", &out)); // INT32_MIN - 1

    EXPECT_FALSE(parse_i32("abc", &out));
    EXPECT_FALSE(parse_i32(NULL, &out));
    EXPECT_FALSE(parse_i32("", &out));
    EXPECT_FALSE(parse_i32("42", NULL));
}

//----------------------------------------------------------------------------//
// parse_u64 tests
//----------------------------------------------------------------------------//

TEST(ParseU64Test, ValidInputs) {
    uint64_t out;

    EXPECT_TRUE(parse_u64("0", &out)); EXPECT_EQ(out, 0ULL);
    EXPECT_TRUE(parse_u64("18446744073709551615", &out)); // UINT64_MAX
    EXPECT_EQ(out, UINT64_MAX);
    EXPECT_TRUE(parse_u64("0001", &out)); EXPECT_EQ(out, 1ULL);
    EXPECT_TRUE(parse_u64("42", &out)); EXPECT_EQ(out, 42ULL);
}

TEST(ParseU64Test, InvalidInputs) {
    uint64_t out;

    // Overflow: UINT64_MAX + 1 (18446744073709551616)
    EXPECT_FALSE(parse_u64("18446744073709551616", &out));

    EXPECT_FALSE(parse_u64("abc", &out));
    EXPECT_FALSE(parse_u64(NULL, &out));
    EXPECT_FALSE(parse_u64("", &out));
    EXPECT_FALSE(parse_u64("184467440737095516150", &out)); // extra digit
    EXPECT_FALSE(parse_u64("42", NULL));
}

//----------------------------------------------------------------------------//
// parse_i64 tests
//----------------------------------------------------------------------------//

TEST(ParseI64Test, ValidInputs) {
    int64_t out;

    EXPECT_TRUE(parse_i64("0", &out)); EXPECT_EQ(out, 0LL);
    EXPECT_TRUE(parse_i64("-9223372036854775808", &out)); // INT64_MIN
    EXPECT_EQ(out, INT64_MIN);
    EXPECT_TRUE(parse_i64("9223372036854775807", &out));  // INT64_MAX
    EXPECT_EQ(out, INT64_MAX);
    EXPECT_TRUE(parse_i64("-1", &out)); EXPECT_EQ(out, -1LL);
    EXPECT_TRUE(parse_i64("+42", &out)); EXPECT_EQ(out, 42LL);
}

TEST(ParseI64Test, InvalidInputs) {
    int64_t out;

    // Overflow cases:
    EXPECT_FALSE(parse_i64("9223372036854775808", &out));  // INT64_MAX + 1
    EXPECT_FALSE(parse_i64("-9223372036854775809", &out)); // INT64_MIN - 1

    EXPECT_FALSE(parse_i64("abc", &out));
    EXPECT_FALSE(parse_i64(NULL, &out));
    EXPECT_FALSE(parse_i64("", &out));
    EXPECT_FALSE(parse_i64("42", NULL));
}

//----------------------------------------------------------------------------//
// parse_boolean_token tests
//----------------------------------------------------------------------------//

TEST(ParseBooleanTest, ValidInputs) {
    bool out;

    EXPECT_TRUE(parse_boolean_token("true", &out)); EXPECT_TRUE(out);
    EXPECT_TRUE(parse_boolean_token("false", &out)); EXPECT_FALSE(out);
    EXPECT_TRUE(parse_boolean_token("1", &out)); EXPECT_TRUE(out);
    EXPECT_TRUE(parse_boolean_token("0", &out)); EXPECT_FALSE(out);

    // Case-insensitive variants
    EXPECT_TRUE(parse_boolean_token("TRUE", &out)); EXPECT_TRUE(out);
    EXPECT_TRUE(parse_boolean_token("False", &out)); EXPECT_FALSE(out);
    EXPECT_TRUE(parse_boolean_token("TrUe", &out)); EXPECT_TRUE(out);
    EXPECT_TRUE(parse_boolean_token("FaLsE", &out)); EXPECT_FALSE(out);
}

TEST(ParseBooleanTest, InvalidInputs) {
    bool out;

    EXPECT_FALSE(parse_boolean_token("yes", &out));
    EXPECT_FALSE(parse_boolean_token(NULL, &out));
    EXPECT_FALSE(parse_boolean_token("false", NULL));
    EXPECT_FALSE(parse_boolean_token("", &out));
}

//----------------------------------------------------------------------------//
// parse_hex_string tests
//----------------------------------------------------------------------------//

TEST(ParseHexStringTest, ValidInputs) {
    uint8_t out[4] = {0};
    size_t out_len = 0;

    EXPECT_TRUE(parse_hex_string("01FF", out, 2, &out_len));
    EXPECT_EQ(out_len, 2u);
    EXPECT_EQ(out[0], 0x01);
    EXPECT_EQ(out[1], 0xFF);

    EXPECT_TRUE(parse_hex_string("00", out, 1, &out_len));
    EXPECT_EQ(out_len, 1u);
    EXPECT_EQ(out[0], 0x00);

    EXPECT_TRUE(parse_hex_string("ff", out, 1, &out_len));
    EXPECT_EQ(out_len, 1u);
    EXPECT_EQ(out[0], 0xFF);

    EXPECT_TRUE(parse_hex_string("AAbb", out, 2, &out_len));
    EXPECT_EQ(out_len, 2u);
    EXPECT_EQ(out[0], 0xAA);
    EXPECT_EQ(out[1], 0xBB);

    // Test lowercase hex letters only
    EXPECT_TRUE(parse_hex_string("deadbeef", out, 4, &out_len));
    EXPECT_EQ(out_len, 4u);
    EXPECT_EQ(out[0], 0xDE);
    EXPECT_EQ(out[1], 0xAD);
    EXPECT_EQ(out[2], 0xBE);
    EXPECT_EQ(out[3], 0xEF);
}

TEST(ParseHexStringTest, InvalidInputs) {
    uint8_t out[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    size_t out_len = 0;

    EXPECT_FALSE(parse_hex_string("0G", out, 1, &out_len));     // invalid char
    EXPECT_FALSE(parse_hex_string("123", out, 2, &out_len));    // odd length
    EXPECT_FALSE(parse_hex_string("AABBCC", out, 2, &out_len)); // too long for buffer
    EXPECT_FALSE(parse_hex_string(NULL, out, 2, &out_len));     // null input string
    EXPECT_FALSE(parse_hex_string("FF", NULL, 1, &out_len));    // null output buffer
    EXPECT_FALSE(parse_hex_string("", out, 2, &out_len));       // empty string
    EXPECT_FALSE(parse_hex_string("FF", out, 0, &out_len));     // zero max_len
    EXPECT_FALSE(parse_hex_string("FF", out, 1, NULL));         // null out_len pointer
}

TEST(ParseHexStringTest, LongerHexArrays) {
    uint8_t out[16] = {0};
    size_t out_len = 0;

    // 16 bytes max (32 hex chars)
    const char *hex_32 = "00112233445566778899AABBCCDDEEFF";
    EXPECT_TRUE(parse_hex_string(hex_32, out, 16, &out_len));
    EXPECT_EQ(out_len, 16u);
    uint8_t expected_16[16] = {
        0x00, 0x11, 0x22, 0x33,
        0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xAA, 0xBB,
        0xCC, 0xDD, 0xEE, 0xFF
    };
    EXPECT_TRUE(std::equal(out, out + 16, expected_16));

    // Test full buffer usage exactly at max length
    char hex_max[2*16 + 1] = {0};
    for (size_t i = 0; i < 16; ++i) {
        snprintf(&hex_max[i*2], 3, "%02X", (unsigned)i);
    }
    EXPECT_TRUE(parse_hex_string(hex_max, out, 16, &out_len));
    EXPECT_EQ(out_len, 16u);
    for (size_t i = 0; i < 16; ++i) {
        EXPECT_EQ(out[i], (uint8_t)i);
    }

    // Test hex string just 1 byte too long (overflow)
    const char *hex_34 = "00112233445566778899AABBCCDDEEFF00";
    EXPECT_FALSE(parse_hex_string(hex_34, out, 16, &out_len));
}

TEST(ParseHexStringTest, EdgeCases) {
    uint8_t out[8] = {0};
    size_t out_len = 0;

    // Minimal valid input: "00"
    EXPECT_TRUE(parse_hex_string("00", out, 1, &out_len));
    EXPECT_EQ(out_len, 1);
    EXPECT_EQ(out[0], 0x00);

    // Max buffer size (8 bytes) exact
    EXPECT_TRUE(parse_hex_string("0102030405060708", out, 8, &out_len));
    EXPECT_EQ(out_len, 8);
    EXPECT_EQ(out[7], 0x08);

    // Max buffer size exceeded by 1 byte (9 bytes)
    EXPECT_FALSE(parse_hex_string("010203040506070809", out, 8, &out_len));

    // Odd length string with valid hex digits but invalid length
    EXPECT_FALSE(parse_hex_string("ABC", out, 2, &out_len));

    // Invalid characters with correct length
    EXPECT_FALSE(parse_hex_string("0G", out, 1, &out_len));
}
