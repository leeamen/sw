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

#include "target.h"

#include <sw/builder/os.h>
#include <sw/manager/sw_context.h>

namespace sw
{

struct IDriver;
struct Input;
struct InputDatabase;
struct SwBuild;

// core context for drivers
struct SW_CORE_API SwCoreContext : SwManagerContext
{
    SwCoreContext(const path &local_storage_root_dir, bool allow_network);
    virtual ~SwCoreContext();

    // from old builder ctx
    const OS &getHostOs() const { return HostOS; }

    TargetMap &getPredefinedTargets() { return predefined_targets; }
    const TargetMap &getPredefinedTargets() const { return predefined_targets; }

    const std::unordered_map<PackageId, TargetData> &getTargetData() const { return target_data; }
    TargetData &getTargetData(const PackageId &);
    const TargetData &getTargetData(const PackageId &) const;

    void setHostSettings(const TargetSettings &s);
    /// these host settings may be changed by the user
    const TargetSettings &getHostSettings() const { return host_settings; }
    /// original, unmodified host settings
    TargetSettings createHostSettings() const;

    InputDatabase &getInputDatabase();
    InputDatabase &getInputDatabase() const;

private:
    // rename to detected?
    // not only detected, but also predefined? do not rename?
    OS HostOS;
    TargetMap predefined_targets;
    std::unordered_map<PackageId, TargetData> target_data;
    TargetSettings host_settings;
    std::unique_ptr<InputDatabase> idb;
};

// public context
struct SW_CORE_API SwContext : SwCoreContext
{
    using Drivers = std::map<PackageId, std::unique_ptr<IDriver>>;

    SwContext(const path &local_storage_root_dir, bool allow_network);
    virtual ~SwContext();

    void registerDriver(const PackageId &pkg, std::unique_ptr<IDriver> &&driver);
    //const Drivers &getDrivers() const { return drivers; }

    std::unique_ptr<SwBuild> createBuild();
    void executeBuild(const path &);

    // stops current operation
    SwBuild *registerOperation(SwBuild *);
    void stop(std::thread::id);

    //
    std::vector<std::unique_ptr<Input>> detectInputs(const path &) const;
    Input *getInput(size_t hash) const;
    std::vector<Input *> addInputInternal(const path &);
    //                inserted
    std::pair<Input *, bool> registerInput(std::unique_ptr<Input>);

    void loadEntryPointsBatch(const std::set<Input*> &inputs);

    const TargetSettings &getSettings() const { return settings; }
    void setSettings(const TargetSettings &s) { settings = s; }

private:
    using InputPtr = std::unique_ptr<Input>;
    using Inputs = std::map<size_t, InputPtr>;

    Drivers drivers;
    Inputs inputs;
    TargetSettings settings;
    std::mutex m;
    std::map<std::thread::id, SwBuild *> active_operations;

    std::unique_ptr<SwBuild> createBuild1();

};

} // namespace sw
