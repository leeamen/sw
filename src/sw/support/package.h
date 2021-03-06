// Copyright (C) 2016-2019 Egor Pugin <egor.pugin@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "enums.h"
#include "package_id.h"

namespace sw
{

struct IStorage;

struct SW_SUPPORT_API PackageData
{
    // internal id?
    //db::PackageVersionId id = 0;

    SomeFlags flags;

    // source package hash (sw.tar.gz)
    String hash;
    String source;

    // length of prefix path
    // e.g., if package path is 'myproject.pkg' and it's added to 'org.sw',
    // then prefix equals to size of 'org.sw', thus 2
    int prefix = 2;

    //
    UnresolvedPackages dependencies;

    // for overridden pkgs
    path sdir;

    //
    // PackageId driver

    virtual ~PackageData() = default;

    virtual std::unique_ptr<PackageData> clone() const { return std::make_unique<PackageData>(*this); }

    virtual String getHash(StorageFileType type, size_t config_hash = 0) const;
};

using PackageDataPtr = std::unique_ptr<PackageData>;

struct SW_SUPPORT_API Package : PackageId
{
    Package(const IStorage &, const PackageId &);

    Package(const Package &);
    Package &operator=(const Package &) = delete;
    Package(Package &&) = default;
    Package &operator=(Package &&) = delete;
    virtual ~Package() = default;

    String getHash() const;
    String getHashShort() const;
    path getHashPath() const;

    const PackageData &getData() const;
    const IStorage &getStorage() const;

    virtual std::unique_ptr<Package> clone() const { return std::make_unique<Package>(*this); }

private:
    const IStorage &storage;
    mutable PackageDataPtr data;
};

using PackagePtr = std::unique_ptr<Package>;
//using Packages = std::unordered_set<Package>;

SW_SUPPORT_API
String getSourceDirectoryName();

}

namespace std
{

template<> struct hash<::sw::Package>
{
    size_t operator()(const ::sw::Package &p) const
    {
        return std::hash<::sw::PackageId>()(p);
    }
};

}
