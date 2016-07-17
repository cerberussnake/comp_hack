/**
 * @file libcomp/tests/Convert.cpp
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Character set conversion tests.
 *
 * This file is part of the COMP_hack Library (libcomp).
 *
 * Copyright (C) 2016 COMP_hack Team <compomega@tutanota.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <PushIgnore.h>
#include <gtest/gtest.h>
#include <PopIgnore.h>

#include <Convert.h>

using namespace libcomp;


TEST(String, CP1252)
{
    String decodedString = "This is CP-1252 encoding: ©ÆüØ";

    EXPECT_EQ(decodedString.Length(), 30);
    unsigned char encodedString[] = {
        0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x43, 0x50, 0x2d, 0x31,
        0x32, 0x35, 0x32, 0x20, 0x65, 0x6e, 0x63, 0x6f, 0x64, 0x69, 0x6e, 0x67,
        0x3a, 0x20, 0xa9, 0xc6, 0xfc, 0xd8, 0x00
    };

    std::vector<char> encodedData(reinterpret_cast<char*>(encodedString),
        reinterpret_cast<char*>(encodedString) + sizeof(encodedString));

    std::vector<char> testData = Convert::ToEncoding(Convert::ENCODING_CP1252,
        decodedString);

    ASSERT_EQ(testData.size(), encodedData.size());

    EXPECT_EQ(memcmp(encodedString, &testData[0],
        sizeof(encodedString)), 0);
    EXPECT_EQ(Convert::FromEncoding(Convert::ENCODING_CP1252, &encodedData[0]),
        decodedString);

    testData = Convert::ToEncoding(Convert::ENCODING_CP1252,
        decodedString, false);

    ASSERT_EQ(testData.size(), encodedData.size() - 1);

    EXPECT_EQ(memcmp(encodedString, &testData[0],
        sizeof(encodedString) - 1), 0);
    EXPECT_EQ(Convert::FromEncoding(Convert::ENCODING_CP1252, &encodedData[0],
        sizeof(encodedString) - 1), decodedString);
    EXPECT_EQ(Convert::SizeEncoded(Convert::ENCODING_CP1252, decodedString),
        sizeof(encodedString) - 1);
    EXPECT_EQ(Convert::SizeEncoded(Convert::ENCODING_CP1252, decodedString, 4),
        ((sizeof(encodedString) - 1 + 4 - 1) / 4) * 4);
}

TEST(String, CP932)
{
    String decodedString = "This is CP-932 encoding: 日本語が大好き！";

    unsigned char encodedString[] = {
        0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x43, 0x50, 0x2d, 0x39,
        0x33, 0x32, 0x20, 0x65, 0x6e, 0x63, 0x6f, 0x64, 0x69, 0x6e, 0x67, 0x3a,
        0x20, 0x93, 0xfa, 0x96, 0x7b, 0x8c, 0xea, 0x82, 0xaa, 0x91, 0xe5, 0x8d,
        0x44, 0x82, 0xab, 0x81, 0x49, 0x00
    };

    std::vector<char> encodedData(reinterpret_cast<char*>(encodedString),
        reinterpret_cast<char*>(encodedString) + sizeof(encodedString));

    std::vector<char> testData = Convert::ToEncoding(Convert::ENCODING_CP932,
        decodedString);

    ASSERT_EQ(testData.size(), encodedData.size());

    EXPECT_EQ(memcmp(encodedString, &testData[0],
        sizeof(encodedString)), 0);
    EXPECT_EQ(Convert::FromEncoding(Convert::ENCODING_CP932, &encodedData[0]),
        decodedString);

    testData = Convert::ToEncoding(Convert::ENCODING_CP932,
        decodedString, false);

    ASSERT_EQ(testData.size(), encodedData.size() - 1);

    EXPECT_EQ(memcmp(encodedString, &testData[0],
        sizeof(encodedString) - 1), 0);
    EXPECT_EQ(Convert::FromEncoding(Convert::ENCODING_CP932, &encodedData[0],
        sizeof(encodedString) - 1), decodedString);
    EXPECT_EQ(Convert::SizeEncoded(Convert::ENCODING_CP932, decodedString),
        sizeof(encodedString) - 1);
    EXPECT_EQ(Convert::SizeEncoded(Convert::ENCODING_CP932, decodedString, 4),
        ((sizeof(encodedString) - 1 + 4 - 1) / 4) * 4);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
