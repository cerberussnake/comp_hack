/**
 * @file libcomp/src/DatabaseSQLite3.h
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

#ifndef LIBCOMP_SRC_DATABASESQLITE3_H
#define LIBCOMP_SRC_DATABASESQLITE3_H

#include "Database.h"

typedef struct sqlite3 sqlite3;

namespace libcomp
{

class DatabaseSQLite3 : public Database
{
public:
    DatabaseSQLite3();
    virtual ~DatabaseSQLite3();

    virtual bool Open(const String& address, const String& username = String(),
        const String& password = String());
    virtual bool Close();
    virtual bool IsOpen() const;

private:
    sqlite3 *mDatabase;
};

} // namespace libcomp

#endif // LIBCOMP_SRC_DATABASESQLITE3_H
