/* SPDX-License-Identifier: Apache-2.0 */

#include "gtest/gtest.h"

extern "C" {
#include "ce_hash.h"
}

TEST(CeHashTest, KnownHashValues)
{
    EXPECT_EQ(ce_hash_calculate("set_speed"),   0x435A0D81);  // example value
    EXPECT_EQ(ce_hash_calculate("set_mac"),     0x8C2AFEA1);  // example value
    EXPECT_EQ(ce_hash_calculate("reset"),       0x10474288);  // example value
}

TEST(CeHashTest, EmptyString)
{
    EXPECT_EQ(ce_hash_calculate(""), 5381u);  // base value of DJB2
}

TEST(CeHashTest, CaseSensitivity)
{
    EXPECT_NE(ce_hash_calculate("Reset"), ce_hash_calculate("reset"));
}
