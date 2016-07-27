/**
 * @file libcomp/tests/Database.cpp
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Test the SQLite3 database backend.
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

#include <DatabaseSQLite3.h>

using namespace libcomp;

TEST(SQLite3, OpenCloseDatabase)
{
    DatabaseSQLite3 db;

    ASSERT_TRUE(db.Open(":memory:"));
    ASSERT_TRUE(db.IsOpen());
    ASSERT_TRUE(db.Close());
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
