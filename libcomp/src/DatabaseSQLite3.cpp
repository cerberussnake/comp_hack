/**
 * @file libcomp/src/DatabaseSQLite3.cpp
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Class to handle an SQLite3 database.
 *
 * This file is part of the COMP_hack Library (libcomp).
 *
 * Copyright (C) 2012-2016 COMP_hack Team <compomega@tutanota.com>
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

#include "DatabaseSQLite3.h"

// libcomp Includes
#include "Log.h"

// SQLite3 Includes
#include <sqlite3.h>

using namespace libcomp;

DatabaseSQLite3::DatabaseSQLite3() : mDatabase(nullptr)
{
}

DatabaseSQLite3::~DatabaseSQLite3()
{
    Close();
}

bool DatabaseSQLite3::Open(const String& address, const String& username,
    const String& password)
{
    (void)username;
    (void)password;

    bool result = true;

    if(SQLITE_OK != sqlite3_open(address.C(), &mDatabase))
    {
        result = false;

        LOG_ERROR(String("Failed to open database: %1\n").Arg(
            sqlite3_errmsg(mDatabase)));

        (void)Close();
    }

    return result;
}

bool DatabaseSQLite3::Close()
{
    bool result = true;

    if(nullptr != mDatabase)
    {
        if(SQLITE_OK != sqlite3_close(mDatabase))
        {
            result = false;

            LOG_ERROR("Failed to close database.\n");
        }

        mDatabase = nullptr;
    }

    return result;
}

bool DatabaseSQLite3::IsOpen() const
{
    return nullptr != mDatabase;
}
