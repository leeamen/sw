// Copyright (C) 2017-2018 Egor Pugin <egor.pugin@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "concurrent_map.h"
#include "file.h"

#include <primitives/templates.h>

namespace primitives::filesystem
{

class FileMonitor;

}

namespace sw
{

struct SW_BUILDER_API FileStorage
{
    struct file_holder
    {
        ScopedFile f;
        path fn;

        file_holder(const path &fn);
        ~file_holder();
    };

    String config;
    ConcurrentHashMap<path, FileRecord> files;

    FileStorage(const String &config);
    FileStorage(const FileStorage &) = delete;
    FileStorage &operator=(const FileStorage &) = delete;
    ~FileStorage();

    void load();
    void save();
    void reset();

    FileRecord *registerFile(const File &f);
    FileRecord *registerFile(const path &f);

    void async_file_log(const FileRecord *r);
    void async_command_log(size_t hash, size_t lwt);

private:
    std::unique_ptr<file_holder> async_file_log_;
    std::unique_ptr<file_holder> async_command_log_;

    file_holder *getFileLog();
    file_holder *getCommandLog();
};

SW_BUILDER_API
FileStorage &getFileStorage(const String &config);

SW_BUILDER_API
FileStorage &getServiceFileStorage();

using FileStorages = std::map<String, FileStorage>;

SW_BUILDER_API
SW_DECLARE_GLOBAL_STATIC_FUNCTION(FileStorages, getFileStorages);

SW_BUILDER_API
SW_DECLARE_GLOBAL_STATIC_FUNCTION(primitives::filesystem::FileMonitor, getFileMonitor);

using FileDataHashMap = ConcurrentHashMap<path, FileData>;

SW_BUILDER_API
SW_DECLARE_GLOBAL_STATIC_FUNCTION(FileDataHashMap, getFileData);

}
