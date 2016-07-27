/**
 * @file libcomp/tests/Packet.cpp
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Test the Packet class.
 *
 * This file is part of the COMP_hack Library (libcomp).
 *
 * Copyright (C) 2014-2016 COMP_hack Team <compomega@tutanota.com>
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

#include <Packet.h>

using namespace libcomp;

TEST(Packet, MoveConstructor)
{
    Packet a;
    a.WriteArray("abc", 3);

    Packet b(std::move(a));
    a.WriteArray("z", 1);
    a.Rewind();

    EXPECT_EQ(b.Size(), 3);
    EXPECT_EQ(a.Size(), 1);
    EXPECT_EQ(b.Tell(), 3);
    EXPECT_EQ(a.Tell(), 0);

    b.Rewind();

    EXPECT_EQ(String(&b.ReadArray(3)[0], 3), "abc");
    EXPECT_EQ(String(&a.ReadArray(1)[0], 1), "z");
}

TEST(Packet, MoveAssignment)
{
    Packet a;
    a.WriteArray("abc", 3);

    Packet b;
    b.WriteArray("defg", 4);
    b = std::move(a);
    a.WriteArray("z", 1);
    a.Rewind();

    EXPECT_EQ(b.Size(), 3);
    EXPECT_EQ(a.Size(), 1);
    EXPECT_EQ(b.Tell(), 3);
    EXPECT_EQ(a.Tell(), 0);

    b.Rewind();

    EXPECT_EQ(String(&b.ReadArray(3)[0], 3), "abc");
    EXPECT_EQ(String(&a.ReadArray(1)[0], 1), "z");
}

int main(int argc, char *argv[])
{
    try
    {
        ::testing::InitGoogleTest(&argc, argv);

        return RUN_ALL_TESTS();
    }
    catch(...)
    {
        return EXIT_FAILURE;
    }
}
