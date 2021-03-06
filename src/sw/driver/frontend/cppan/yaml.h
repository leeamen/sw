/*
 * SW - Build System and Package Manager
 * Copyright (C) 2016-2020 Egor Pugin
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

#include <sw/support/filesystem.h>

#include <primitives/yaml.h>

namespace sw::cppan
{

yaml load_yaml_config(const path &p);
yaml load_yaml_config(const String &s);
yaml load_yaml_config(yaml &root);

void dump_yaml_config(const path &p, const yaml &root);
String dump_yaml_config(const yaml &root);

}
