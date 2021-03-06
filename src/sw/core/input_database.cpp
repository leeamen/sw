/*
 * SW - Build System and Package Manager
 * Copyright (C) 2020 Egor Pugin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "input_database.h"

#include "specification.h"

#include <db_inputs.h>
#include "inserts.h"
#include <sqlpp11/sqlite3/connection.h>
#include <sqlpp11/sqlite3/sqlite3.h>
#include <sqlpp11/sqlpp11.h>

namespace sw
{

InputDatabase::InputDatabase(const path &p)
    : Database(p, inputs_db_schema)
{
}

size_t InputDatabase::getFileHash(const path &p) const
{
    const ::db::inputs::File file{};

    bool update_db = false;
    auto lwt = fs::last_write_time(p);
    auto np = normalize_path(p);

    auto q = (*db)(
        select(file.hash, file.lastWriteTime)
        .from(file)
        .where(file.path == np));
    if (!q.empty())
    {
        if (memcmp(q.front().lastWriteTime.value().data(), &lwt, sizeof(lwt)) == 0)
            return q.front().hash.value();
        update_db = true;
    }

    auto c = read_file(p);
    auto h = std::hash<String>()(c);

    std::vector<uint8_t> lwtdata(sizeof(lwt));
    memcpy(lwtdata.data(), &lwt, lwtdata.size());
    if (update_db)
    {
        (*db)(update(file).set(
            file.hash = h,
            file.lastWriteTime = lwtdata
        ).where(file.path == np));
    }
    else
    {
        (*db)(insert_into(file).set(
            file.path = np,
            file.hash = h,
            file.lastWriteTime = lwtdata
        ));
    }

    return h;
}

} // namespace sw
