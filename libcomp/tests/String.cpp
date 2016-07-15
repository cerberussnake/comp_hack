/**
 * @file libcomp/tests/String.cpp
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Unicode string class unit tests.
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

#include <String.h>

using namespace libcomp;

TEST(String, Length)
{
    EXPECT_EQ(9, String("今日は月曜日です。").Length());
}

TEST(String, Size)
{
    EXPECT_EQ(strlen("今日は月曜日です。"), String("今日は月曜日です。").Size());
}

TEST(String, Compare)
{
    EXPECT_EQ("今日は月曜日です。", String("今日は月曜日です。"));
    EXPECT_EQ(String("今日は月曜日です。"), "今日は月曜日です。");

    EXPECT_NE("今日は月曜日", String("今日は月曜日です。"));
    EXPECT_NE(String("今日は月曜日です。"), "今日は月曜日");

    EXPECT_NE("今日は月曜日です。", String("今日は月曜日"));
    EXPECT_NE(String("今日は月曜日"), "今日は月曜日です。");

    EXPECT_EQ(std::string("今日は月曜日です。"), String("今日は月曜日です。"));
    EXPECT_EQ(String("今日は月曜日です。"), std::string("今日は月曜日です。"));

    EXPECT_NE(std::string("今日は月曜日"), String("今日は月曜日です。"));
    EXPECT_NE(String("今日は月曜日です。"), std::string("今日は月曜日"));

    EXPECT_NE(std::string("今日は月曜日です。"), String("今日は月曜日"));
    EXPECT_NE(String("今日は月曜日"), std::string("今日は月曜日です。"));
}

TEST(String, Right)
{
    EXPECT_EQ("def", String("abcdef").Right(3)) <<
        "String.Right() does not return correct characters.";

    EXPECT_EQ("月曜日です。", String("今日は月曜日です。").Right(6)) <<
        "String.Right() does not return correct characters.";

    EXPECT_EQ("今日は月曜日です。", String("今日は月曜日です。").Right(100)) <<
        "String.Right() does not return entire string.";

    EXPECT_EQ("", String("今日は月曜日です。").Right(0)) <<
        "String.Right() does not return an empty string.";

    EXPECT_EQ("", String("").Right(10)) <<
        "String.Right() does not return an empty string.";
}

TEST(String, Left)
{
    EXPECT_EQ("abc", String("abcdef").Left(3)) <<
        "String.Left() does not return correct characters.";

    EXPECT_EQ("今日は月曜日", String("今日は月曜日です。").Left(6)) <<
        "String.Left() does not return correct characters.";

    EXPECT_EQ("今日は月曜日です。", String("今日は月曜日です。").Left(100)) <<
        "String.Left() does not return entire string.";

    EXPECT_EQ("", String("今日は月曜日です。").Left(0)) <<
        "String.Left() does not return an empty string.";

    EXPECT_EQ("", String("").Left(10)) <<
        "String.Left() does not return an empty string.";
}

TEST(String, Truncate)
{
    String s;

    s = String("abcdef");
    s.Truncate(3);

    EXPECT_EQ("abc", s) <<
        "String.Truncate() does not return correct characters.";

    s = String("今日は月曜日です。");
    s.Truncate(6);

    EXPECT_EQ("今日は月曜日", s) <<
        "String.Truncate() does not return correct characters.";

    s = String("今日は月曜日です。");
    s.Truncate(100);

    EXPECT_EQ("今日は月曜日です。", s) <<
        "String.Truncate() does not return entire string.";

    s = String("今日は月曜日です。");
    s.Truncate(0);

    EXPECT_EQ("", s) <<
        "String.Truncate() does not return an empty string.";

    s = String("");
    s.Truncate(10);

    EXPECT_EQ("", s) <<
        "String.Truncate() does not return an empty string.";
}

TEST(String, Mid)
{
    EXPECT_EQ("は月曜日", String("今日は月曜日です。").Mid(2, 4)) <<
        "String.Mid() does not return correct characters.";
    EXPECT_EQ("は月曜日です。", String("今日は月曜日です。").Mid(2)) <<
        "String.Mid() does not return correct characters.";
    EXPECT_EQ("は月曜日です。", String("今日は月曜日です。").Mid(2, 7)) <<
        "String.Mid() does not return correct characters.";
    EXPECT_EQ("は月曜日です。", String("今日は月曜日です。").Mid(2, 8)) <<
        "String.Mid() does not return correct characters.";
    EXPECT_EQ("今日は月曜日です。", String("今日は月曜日です。").Mid(0)) <<
        "String.Mid() does not return correct characters.";
}

TEST(String, LeftJustified)
{
    EXPECT_EQ(15, String("今日は月曜日です。").LeftJustified(15, '_').Length()) <<
        "String.LeftJustified() does not return correct number of characters.";

    EXPECT_EQ("今日は月曜日です。aaaaaa",
        String("今日は月曜日です。").LeftJustified(15, 'a')) <<
        "String.LeftJustified() does not return correct characters.";

    EXPECT_EQ("今日は月曜日です。^^^^^^",
        String("今日は月曜日です。").LeftJustified(15, '^')) <<
        "String.LeftJustified() does not return correct characters.";

    EXPECT_EQ("今日は月曜日です。",
        String("今日は月曜日です。").LeftJustified(3, 'a')) <<
        "String.LeftJustified() does not return correct characters.";

    EXPECT_EQ("今日は", String("今日は月曜日です。").LeftJustified(3, 'a', true)) <<
        "String.LeftJustified() does not return correct characters.";
}

TEST(String, RightJustified)
{
    EXPECT_EQ(15, String("今日は月曜日です。").RightJustified(15, '_').Length()) <<
        "String.RightJustified() does not return correct number of characters.";

    EXPECT_EQ("aaaaaa今日は月曜日です。",
        String("今日は月曜日です。").RightJustified(15, 'a')) <<
        "String.RightJustified() does not return correct characters.";

    EXPECT_EQ("^^^^^^今日は月曜日です。",
        String("今日は月曜日です。").RightJustified(15, '^')) <<
        "String.RightJustified() does not return correct characters.";

    EXPECT_EQ("今日は月曜日です。",
        String("今日は月曜日です。").RightJustified(3, 'a')) <<
        "String.RightJustified() does not return correct characters.";

    EXPECT_EQ("今日は", String("今日は月曜日です。").RightJustified(3, 'a', true)) <<
        "String.RightJustified() does not return correct characters.";
}

TEST(String, Append)
{
    {
        String a = "今日は";
        String b = "月曜日です。";
        String c = a;
        c += b;

        EXPECT_EQ("今日は", a);
        EXPECT_EQ("月曜日です。", b);
        EXPECT_EQ("今日は月曜日です。", c);
    }

    {
        String a = "初めまして。";
        String b = "僕はオメガです。";
        String c = a;
        c.Append(b);

        EXPECT_EQ("初めまして。", a);
        EXPECT_EQ("僕はオメガです。", b);
        EXPECT_EQ("初めまして。僕はオメガです。", c);
    }
}

TEST(String, Prepend)
{
    String a = std::string("今日は");
    String b = std::string("月曜日です。");
    String c = b;
    c.Prepend(a);

    EXPECT_EQ("今日は", a);
    EXPECT_EQ("月曜日です。", b);
    EXPECT_EQ("今日は月曜日です。", c);
}

TEST(String, Arguments)
{
    EXPECT_EQ("Arguments: a1, b2, c3", String("Arguments: %2, %1, %3").Arg(
        "b2").Arg("a1").Arg("c3"));

    std::stringstream ss, ss2;
    ss << "Arguments: %100";
    ss2 << "Arguments: %100";

    for(int i = 1; i <= 99; ++i)
    {
        ss << " %" << i;
        ss2 << " " << (100 - i);
    }

    String s(ss.str());

    for(int i = 1; i <= 99; ++i)
    {
        std::stringstream a;
        a << (100 - i);

        s = s.Arg(a.str());
    }

    EXPECT_EQ(ss2.str(), s.ToUtf8());

    EXPECT_EQ("a b a", String("%1 %2 %1").Arg("a").Arg("b"));

    bool reporting = String::IsReportingBadArguments();
    String::SetBadArgumentReporting(false);
    EXPECT_EQ("Argument 1 is missing: b",
        String("Argument 1 is missing: %2").Arg("a").Arg("b"));
    String::SetBadArgumentReporting(reporting);
}

TEST(String, ArgInt)
{
    EXPECT_EQ("123", String("%1").Arg(123));
    EXPECT_EQ("0x00ff", String("0x%1").Arg(255, 4, 16, '0'));
}

TEST(String, ToUpperLower)
{
    EXPECT_EQ("ABCDEF", String("aBcDeF").ToUpper());
    EXPECT_EQ("abcdef", String("aBcDeF").ToLower());
}

TEST(String, Contains)
{
    EXPECT_TRUE(String("abcdef").Contains("abcdef"));
    EXPECT_TRUE(String("abcdef").Contains("bcde"));
    EXPECT_TRUE(String("abcdef").Contains(""));

    EXPECT_FALSE(String("abcdef").Contains("bob"));
    EXPECT_FALSE(String("abcdef").Contains("abcdef "));
}

TEST(String, Trim)
{
    EXPECT_EQ("abc\r\t \n", String("\t \n\r abc\r\t \n").LeftTrimmed());
    EXPECT_EQ("\t \n\r abc", String("\t \n\r abc\r\t \n").RightTrimmed());
    EXPECT_EQ("abc", String("\t \n\r abc\r\t \n").Trimmed());
}

TEST(String, Clear)
{
    String s = "abc";

    EXPECT_FALSE(s.IsEmpty());
    EXPECT_EQ(3, s.Length());
    EXPECT_EQ(3, s.Size());
    EXPECT_EQ("abc", s);

    s.Clear();

    EXPECT_TRUE(s.IsEmpty());
    EXPECT_EQ(0, s.Length());
    EXPECT_EQ(0, s.Size());
    EXPECT_EQ("", s);
}

TEST(String, Split)
{
    std::list<String> list = String("@!@ace@!@bee@!@cat@!@").Split("@!@");

    ASSERT_EQ(5, list.size());

    EXPECT_EQ("", list.front()); list.pop_front();
    EXPECT_EQ("ace", list.front()); list.pop_front();
    EXPECT_EQ("bee", list.front()); list.pop_front();
    EXPECT_EQ("cat", list.front()); list.pop_front();
    EXPECT_EQ("", list.front()); list.pop_front();
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
