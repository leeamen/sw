/*
 * SW - Build System and Package Manager
 * Copyright (C) 2017-2019 Egor Pugin
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

#include "../commands.h"

#include <sw/manager/storage.h>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#include <Objbase.h>
#include <Shlobj.h>
#endif

#include <primitives/log.h>
DECLARE_STATIC_LOGGER(logger, "command.open");

static void open_nix(const String &p)
{
#ifdef _WIN32
    SW_UNREACHABLE;
#endif
    String s;
#ifdef __linux__
    s += "xdg-";
#endif
    s += "open \"" + normalize_path(p) + "\"";
    if (system(s.c_str()) != 0)
    {
#if !(defined(__linux__) || defined(__APPLE__))
        SW_UNIMPLEMENTED;
#else
        throw SW_RUNTIME_ERROR("Cannot open: " + p);
#endif
    }
}

void open_directory(const path &p)
{
#ifdef _WIN32
    auto pidl = ILCreateFromPath(p.wstring().c_str());
    if (pidl)
    {
        CoInitialize(0);
        // ShellExecute does not work here for some scenarios
        auto r = SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
        if (FAILED(r))
        {
            LOG_INFO(logger, "Error in SHOpenFolderAndSelectItems");
        }
        ILFree(pidl);
    }
    else
    {
        LOG_INFO(logger, "Error in ILCreateFromPath");
    }
#else
    open_nix(p);
#endif
}

void open_file(const path &p)
{
#ifdef _WIN32
    CoInitialize(0);
    auto r = ShellExecute(0, L"open", p.wstring().c_str(), 0, 0, 0);
    if (r <= (HINSTANCE)HINSTANCE_ERROR)
    {
        throw SW_RUNTIME_ERROR("Error in ShellExecute");
    }
#else
    open_nix(p);
#endif
}

void open_url(const String &url)
{
#ifdef _WIN32
    CoInitialize(0);
    auto r = ShellExecute(0, L"open", to_wstring(url).c_str(), 0, 0, 0);
    if (r <= (HINSTANCE)HINSTANCE_ERROR)
    {
        throw SW_RUNTIME_ERROR("Error in ShellExecute");
    }
#else
    open_nix(url);
#endif
}

SUBCOMMAND_DECL(open)
{
    auto &sdb = getContext().getLocalStorage();

    sw::UnresolvedPackages upkgs;
    for (auto &a : getInputs())
        upkgs.insert(a);

    auto pkgs = getContext().resolve(upkgs);
    for (auto &u : upkgs)
    {
        auto ip = pkgs.find(u);
        if (ip == pkgs.end())
        {
            LOG_WARN(logger, "Cannot get " + u.toString());
            continue;
        }
        auto &p = *ip->second;
        if (!sdb.isPackageInstalled(p))
        {
            LOG_INFO(logger, "Package '" + p.toString() + "' not installed");
            continue;
        }

        sw::LocalPackage lp(getContext().getLocalStorage(), p);

        LOG_INFO(logger, "package: " + lp.toString());
        LOG_INFO(logger, "package dir: " + lp.getDir().u8string());

        open_directory(lp.getDirSrc() / ""); // on win we must add last slash
    }
}
