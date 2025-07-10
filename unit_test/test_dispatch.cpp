/* SPDX-License-Identifier: Apache-2.0 */

#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <cstring>
#include <iomanip>

extern "C" {
#include "ce_dispatch.h"
#include "ce_invoke_handler.h"
#include "ce_dispatch_test_api.h"
#include "ce_hash.h"
#include "ce_table.h"
#include "ce_command_ids.h"
#include "demo.h"
}

// -----------------------------------------------------------------------------
// Mock and Test Fixtures
// -----------------------------------------------------------------------------

static std::string g_last_string;
static uint8_t g_last_byte = 0xFF;
static std::vector<uint8_t> g_last_bytes;
static int g_mock_calls = 0;
static bool g_void_called = false;

typedef struct {
    std::vector<uint8_t> u8a1;
    std::vector<uint8_t> u8a2;
    std::string s1;
    std::string s2;
    std::vector<uint8_t> u8a3;
    int64_t i64;
    uint32_t u32;
    int call_count = 0;
} mixed_mock_st;
static mixed_mock_st g_mixed_mock;

extern "C" bool demo_cat_byte(uint8_t value) {
    g_last_byte = value;
    g_mock_calls++;
    return true;
}

extern "C" bool demo_cat_string(const char* str) {
    g_last_string = std::string(str);
    g_mock_calls++;
    return true;
}

extern "C" bool demo_void(void) {
    g_void_called = true;
    g_mock_calls++;
    return true;
}

extern "C" bool demo_cat_bytes(const uint8_t* data, uint8_t len) {
    g_last_bytes.assign(data, data + len);
    g_mock_calls++;
    return true;
}

extern "C" bool demo_cat_mixed(const uint8_t* u8a1,
                               const uint8_t* u8a2,
                               const char* s1,
                               const char* s2,
                               const uint8_t* u8a3,
                               int64_t i64,
                               uint32_t u32) {
    g_mixed_mock.call_count++;
    g_mixed_mock.u8a1.assign(u8a1, u8a1 + 3);
    g_mixed_mock.u8a2.assign(u8a2, u8a2 + 3);
    g_mixed_mock.s1 = std::string(s1);
    g_mixed_mock.s2 = std::string(s2);
    g_mixed_mock.u8a3.assign(u8a3, u8a3 + 4);
    g_mixed_mock.i64 = i64;
    g_mixed_mock.u32 = u32;
    return true;
}

static void reset_mock_state() {
    g_last_string.clear();
    g_last_bytes.clear();
    g_last_byte = 0xFF;
    g_mock_calls = 0;
    g_void_called = false;
}

static void reset_mixed_mock() {
    g_mixed_mock.u8a1.clear();
    g_mixed_mock.u8a2.clear();
    g_mixed_mock.u8a3.clear();
    g_mixed_mock.s1.clear();
    g_mixed_mock.s2.clear();
    g_mixed_mock.i64 = 0;
    g_mixed_mock.u32 = 0;
    g_mixed_mock.call_count = 0;
}

// -----------------------------------------------------------------------------
// Dispatcher End-to-End Tests
// -----------------------------------------------------------------------------

TEST(Dispatch_E2E, VoidCommand) {
    reset_mock_state();
    EXPECT_TRUE(ce_dispatch_from_line("void"));
    EXPECT_EQ(g_mock_calls, 1);
    EXPECT_TRUE(g_void_called);
}

TEST(Dispatch_E2E, CatByte) {
    reset_mock_state();
    EXPECT_TRUE(ce_dispatch_from_line("cat_byte 22"));
    EXPECT_EQ(g_mock_calls, 1);
    EXPECT_EQ(g_last_byte, 22);
}

TEST(Dispatch_E2E, CatString) {
    reset_mock_state();
    EXPECT_TRUE(ce_dispatch_from_line("cat_string hello_world"));
    EXPECT_EQ(g_mock_calls, 1);
    EXPECT_EQ(g_last_string, "hello_world");
}

TEST(Dispatch_E2E, Cat3Bytes) {
    reset_mock_state();
    EXPECT_TRUE(ce_dispatch_from_line("cat_bytes 010203 3"));
    EXPECT_EQ(g_mock_calls, 1);
    EXPECT_EQ(g_last_bytes, (std::vector<uint8_t>{0x01, 0x02, 0x03}));
}

TEST(Dispatch_E2E, Cat7Bytes) {
    reset_mock_state();
    EXPECT_TRUE(ce_dispatch_from_line("cat_bytes 010203AABBCCEE 7"));
    EXPECT_EQ(g_mock_calls, 1);
    EXPECT_EQ(g_last_bytes, (std::vector<uint8_t>{0x01, 0x02, 0x03, 0xAA, 0xBB, 0xCC, 0xEE}));
}

TEST(Dispatch_E2E, CatMixedComplex) {
    reset_mixed_mock();
    const char* cmd_line = "cat_mixed 010203 0A0B0C foo bar D00EFFAA -123456789 987654321";
    EXPECT_TRUE(ce_dispatch_from_line(cmd_line));

    EXPECT_EQ(g_mixed_mock.call_count, 1);
    EXPECT_EQ(g_mixed_mock.u8a1, (std::vector<uint8_t>{0x01, 0x02, 0x03}));
    EXPECT_EQ(g_mixed_mock.u8a2, (std::vector<uint8_t>{0x0A, 0x0B, 0x0C}));
    EXPECT_EQ(g_mixed_mock.s1, "foo");
    EXPECT_EQ(g_mixed_mock.s2, "bar");
    EXPECT_EQ(g_mixed_mock.u8a3, (std::vector<uint8_t>{0xD0, 0x0E, 0xFF, 0xAA}));
    EXPECT_EQ(g_mixed_mock.i64, -123456789LL);
    EXPECT_EQ(g_mixed_mock.u32, 987654321U);
}

// -----------------------------------------------------------------------------
// Argument Parsing Tests
// -----------------------------------------------------------------------------

TEST(ParseArguments, Uint8Parse) {
    reset_mock_state();
    ce_arg_type_et types[] = { TYPE_UINT8_e };
    ce_signature_st sig = { .types_e = types, .type_count_u8 = 1 };
    char* tokens[] = { (char*)"cmd", (char*)"42" };
    ce_arg_value_ut args[MAX_TOKENS] = {};
    EXPECT_TRUE(_test_parse_arguments(&sig, tokens, args));
    EXPECT_EQ(args[0].u8, 42);
}

TEST(ParseArguments, RejectInvalidUint8) {
    reset_mock_state();
    ce_arg_type_et types[] = { TYPE_UINT8_e };
    ce_signature_st sig = { .types_e = types, .type_count_u8 = 1 };
    char* tokens[] = { (char*)"cmd", (char*)"not_a_number" };
    ce_arg_value_ut args[MAX_TOKENS] = {};
    EXPECT_FALSE(_test_parse_arguments(&sig, tokens, args));
}

TEST(ParseArguments, MixedTypesParseCorrectly) {
    ce_arg_type_et types[] = { TYPE_UINT8_e, TYPE_STRING_e, TYPE_UINT32_e };
    ce_signature_st sig = { .types_e = types, .type_count_u8 = 3 };
    char* tokens[] = { (char*)"cmd", (char*)"123", (char*)"abc", (char*)"4294967295" };
    ce_arg_value_ut args[MAX_TOKENS] = {};
    EXPECT_TRUE(_test_parse_arguments(&sig, tokens, args));
    EXPECT_EQ(args[0].u8, 123);
    EXPECT_STREQ(args[1].str_cp, "abc");
    EXPECT_EQ(args[2].u32, 4294967295U);
}

// -----------------------------------------------------------------------------
// Tokenizer Tests
// -----------------------------------------------------------------------------

TEST(ParseTokens, HandlesWhitespace) {
    reset_mock_state();
    char line[] = "   echo   arg1   arg2  ";
    char* tokens[MAX_TOKENS] = {};
    size_t token_count = 0u;
    printf("line strlen : %lu\n", strlen(line));
    EXPECT_TRUE(_test_parse_line_to_tokens(line, tokens, &token_count));
    EXPECT_EQ(token_count, 3);

    EXPECT_STREQ(tokens[0], "echo");
    EXPECT_STREQ(tokens[1], "arg1");
    EXPECT_STREQ(tokens[2], "arg2");
}

TEST(ParseTokens, RejectsTooManyTokens) {
    reset_mock_state();
    char line[1024] = "cmd";
    for (int i = 0; i < MAX_TOKENS; ++i) strcat(line, " arg");
    char* tokens[MAX_TOKENS] = {};
    size_t token_count = 0u;
    EXPECT_FALSE(_test_parse_line_to_tokens(line, tokens, &token_count));
}

// -----------------------------------------------------------------------------
// Edge Case Handling
// -----------------------------------------------------------------------------

TEST(EdgeCases, NullLineHandled) {
    reset_mock_state();
    EXPECT_FALSE(ce_dispatch_from_line(nullptr));
}

TEST(EdgeCases, OneSpaceLineHandled) {
    reset_mock_state();
    EXPECT_FALSE(ce_dispatch_from_line(" "));
    EXPECT_EQ(g_mock_calls, 0);
}

TEST(EdgeCases, ReturnLineHandled) {
    reset_mock_state();
    EXPECT_FALSE(ce_dispatch_from_line("\r"));
    EXPECT_EQ(g_mock_calls, 0);
}

TEST(EdgeCases, EnterLineHandled) {
    reset_mock_state();
    EXPECT_FALSE(ce_dispatch_from_line("\r\n"));
    EXPECT_EQ(g_mock_calls, 0);
}

TEST(EdgeCases, EmptyLineHandled) {
    reset_mock_state();
    EXPECT_FALSE(ce_dispatch_from_line("     "));
    EXPECT_EQ(g_mock_calls, 0);
}

TEST(EdgeCases, UnknownCommandHandled) {
    reset_mock_state();
    EXPECT_FALSE(ce_dispatch_from_line("nonexistent_command"));
    EXPECT_EQ(g_mock_calls, 0);
}

TEST(InvokeHandler, UnknownHashReturnsFalse) {
    ce_signature_st sig = {
        .hash_u32 = 0xDEADBEEF,  // Not in enum
        .handler = (handler_func_t)demo_void,  // doesn't matter
        .types_e = nullptr,
        .type_count_u8 = 0
    };

    ce_arg_value_ut args[MAX_TOKENS] = {};
    EXPECT_FALSE(ce_invoke_handler(&sig, args));
}

TEST(ValidateArgumentCount, NullSignatureReturnsFalse) {
    EXPECT_FALSE(_test_validate_argument_count(3, nullptr));
}

TEST(SignatureLookup, EveryCommandHashResolves) {
    auto count = ce_table_get_signatures_count();
    auto sigs = ce_table_get_signatures();

    for (size_t i = 0; i < count; ++i) {
        const ce_signature_st* match = _test_lookup_signature_by_hash(sigs[i].hash_u32);
        ASSERT_NE(match, nullptr);
        EXPECT_EQ(match->hash_u32, sigs[i].hash_u32);
    }
}
