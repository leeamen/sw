// Copyright (C) 2016-2019 Egor Pugin <egor.pugin@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "package.h"

#include "storage.h"

#include <fstream>

#include <primitives/log.h>
DECLARE_STATIC_LOGGER(logger, "package");

namespace sw
{

static path getHashPathFromHash(const String &h, int nsubdirs, int chars_per_subdir)
{
    path p;
    int i = 0;
    for (; i < nsubdirs; i++)
        p /= h.substr(i * chars_per_subdir, chars_per_subdir);
    p /= h.substr(i * chars_per_subdir);
    return p;
}

Package::Package(const IStorage &storage, const PackageId &id)
    : storage(storage), PackageId(id)
{
}

Package::Package(const Package &rhs)
    : PackageId(rhs), storage(rhs.storage), data(rhs.data ? rhs.data->clone() : nullptr)
{
}

String Package::getHash() const
{
    // move these calculations to storage?
    switch (storage.getSchema().getHashVersion())
    {
    case 1:
        return blake2b_512(getPath().toStringLower() + "-" + getVersion().toString());
    }

    throw SW_RUNTIME_ERROR("Unknown hash schema version: " + std::to_string(storage.getSchema().getHashVersion()));
}

path Package::getHashPath() const
{
    // move these calculations to storage?
    switch (storage.getSchema().getHashPathFromHashVersion())
    {
    case 1:
        return ::sw::getHashPathFromHash(getHash(), 4, 2); // remote consistent storage paths
    case 2:
        return ::sw::getHashPathFromHash(getHashShort(), 2, 2); // local storage is more relaxed
    }

    throw SW_RUNTIME_ERROR("Unknown hash path schema version: " + std::to_string(storage.getSchema().getHashPathFromHashVersion()));
}

String Package::getHashShort() const
{
    return shorten_hash(getHash(), 8);
}

const PackageData &Package::getData() const
{
    if (!data)
        data = std::move(storage.loadData(*this));
    return *data;
}

const IStorage &Package::getStorage() const
{
    return storage;
}

}