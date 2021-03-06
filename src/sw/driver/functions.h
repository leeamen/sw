/*
 * SW - Build System and Package Manager
 * Copyright (C) 2017-2020 Egor Pugin
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

#pragma once

#include <primitives/constants.h>
#include <primitives/filesystem.h>

namespace sw
{

SW_DRIVER_CPP_API
void writeFileOnce(const path &fn, const String &content, const path &lock_dir);

SW_DRIVER_CPP_API
void writeFileSafe(const path &fn, const String &content, const path &lock_dir);

SW_DRIVER_CPP_API
void replaceInFileOnce(const path &fn, const String &from, const String &to, const path &lock_dir);

SW_DRIVER_CPP_API
void pushFrontToFileOnce(const path &fn, const String &text, const path &lock_dir);

SW_DRIVER_CPP_API
void pushBackToFileOnce(const path &fn, const String &text, const path &lock_dir);

SW_DRIVER_CPP_API
bool patch(const path &fn, const String &text, const path &lock_dir);

SW_DRIVER_CPP_API
void downloadFile(const String &url, const path &fn, int64_t file_size_limit = 1_MB);

SW_DRIVER_CPP_API
path getProgramLocation();

}
