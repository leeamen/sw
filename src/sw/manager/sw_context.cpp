// Copyright (C) 2016-2019 Egor Pugin <egor.pugin@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "sw_context.h"

#include "storage.h"

#include <primitives/log.h>
DECLARE_STATIC_LOGGER(logger, "icontext");

namespace sw
{

// remove whn msvc bug is fixed
struct PackageStore {};

SwManagerContext::SwManagerContext(const path &local_storage_root_dir)
{
    auto p = local_storage_root_dir;
    p += "2";
    storages.emplace_back(std::make_unique<LocalStorage>(p));
    storages.emplace_back(std::make_unique<RemoteStorage>(getLocalStorage(), "software-network", getLocalStorage().getDatabaseRootDir()));
}

SwManagerContext::~SwManagerContext() = default;

LocalStorage &SwManagerContext::getLocalStorage()
{
    return static_cast<LocalStorage&>(*storages[0]);
}

const LocalStorage &SwManagerContext::getLocalStorage() const
{
    return static_cast<const LocalStorage&>(*storages[0]);
}

std::unordered_map<UnresolvedPackage, Package> SwManagerContext::resolve(const UnresolvedPackages &pkgs) const
{
    std::lock_guard lk(m);
    return resolve(pkgs, resolved_packages);
}

std::unordered_map<UnresolvedPackage, Package> SwManagerContext::resolve(const UnresolvedPackages &pkgs, std::unordered_map<UnresolvedPackage, Package> &resolved_packages) const
{
    if (pkgs.empty())
        return {};

    std::unordered_map<UnresolvedPackage, Package> resolved;
    auto pkgs2 = pkgs;
    for (auto &s : storages)
    {
        UnresolvedPackages unresolved;
        auto rpkgs = s->resolve(pkgs2, unresolved);
        resolved.merge(rpkgs);
        pkgs2 = std::move(unresolved);
    }
    if (!pkgs2.empty())
    {
        String s;
        for (auto &d : pkgs2)
            s += d.toString() + ", ";
        if (!s.empty())
            s.resize(s.size() - 2);
        throw SW_RUNTIME_ERROR("Some packages (" + std::to_string(pkgs2.size()) + ") are unresolved: " + s);
    }

    return resolved;
}

bool SwManagerContext::isResolved(const UnresolvedPackage &pkg) const
{
    return resolved_packages.find(pkg) != resolved_packages.end();
}

LocalPackage SwManagerContext::resolve(const UnresolvedPackage &pkg) const
{
    return resolve(UnresolvedPackages{ pkg }).find(pkg)->second.install();
}

}
