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

#include "options.h"

#include "target/native.h"

#include <sw/manager/package.h>

#include <boost/algorithm/string.hpp>

#include <tuple>

namespace sw
{

ApiNameType::ApiNameType(const String &s)
{
    a = s;
}

Definition::Definition(const String &s)
{
    d = s;
}

Framework::Framework(const String &s)
{
    f = s;
}

Framework::Framework(const path &p)
{
    f = p.u8string();
}

IncludeDirectory::IncludeDirectory(const String &s)
{
    i = s;
}

IncludeDirectory::IncludeDirectory(const path &p)
{
    i = p.u8string();
}

LinkDirectory::LinkDirectory(const String &s)
{
    d = s;
}

LinkDirectory::LinkDirectory(const path &p)
{
    d = p.u8string();
}

LinkLibrary::LinkLibrary(const String &s)
{
    l = s;
}

LinkLibrary::LinkLibrary(const path &p)
{
    l = p.u8string();
}

SystemLinkLibrary::SystemLinkLibrary(const String &s)
{
    l = s;
}

SystemLinkLibrary::SystemLinkLibrary(const path &p)
{
    l = p.u8string();
}

PrecompiledHeader::PrecompiledHeader(const String &s)
{
    h = s;
}

PrecompiledHeader::PrecompiledHeader(const path &p)
{
    h = p.u8string();
}

FileRegex::FileRegex(const String &fn, bool recursive)
    : recursive(recursive)
{
    // try to extract dirs from string
    size_t p = 0;

    do
    {
        auto p0 = p;
        p = fn.find_first_of("/*?+[.\\", p);
        if (p == -1 || fn[p] != '/')
        {
            regex_string = fn.substr(p0);
            r = regex_string;
            return;
        }

        // scan first part for '\.' pattern that is an exact match for point
        // other patterns? \[ \( \{

        String s = fn.substr(p0, p++ - p0);
        boost::replace_all(s, "\\.", ".");
        boost::replace_all(s, "\\[", "[");
        boost::replace_all(s, "\\]", "]");
        boost::replace_all(s, "\\(", "(");
        boost::replace_all(s, "\\)", ")");
        boost::replace_all(s, "\\{", "{");
        boost::replace_all(s, "\\}", "}");

        if (s.find_first_of("*?+.[](){}") != -1)
        {
            regex_string = fn.substr(p0);
            r = regex_string;
            return;
        }

        dir /= s;
    } while (1);
}

FileRegex::FileRegex(const path &d, const String &fn, bool recursive)
    : FileRegex(fn, recursive)
{
    if (dir.empty())
        dir = d;
    else if (!d.empty())
        dir = d / dir;
}

FileRegex::FileRegex(const std::regex &r, bool recursive)
    : r(r), recursive(recursive)
{
}

FileRegex::FileRegex(const path &d, const std::regex &r, bool recursive)
    : dir(d), r(r), recursive(recursive)
{
}

String FileRegex::getRegexString() const
{
    return normalize_path(dir / "") + regex_string;
}

template <class C>
void unique_merge_containers(C &to, const C &from)
{
    to.insert(from.begin(), from.end());
    /*for (auto &e : c2)
    {
        if (std::find(c1.begin(), c1.end(), e) == c1.end())
            c1.insert(e);
    }*/
}

DependencyData::DependencyData(const ITarget &t)
{
    package = t.getPackage();
}

DependencyData::DependencyData(const UnresolvedPackage &p)
{
    package = p;
}

UnresolvedPackage DependencyData::getPackage() const
{
    /*auto t = target;
    if (t)
        return {t->getPackage().ppath, t->getPackage().version};*/
    return package;
}

PackageId DependencyData::getResolvedPackage() const
{
    if (!target)
        throw SW_RUNTIME_ERROR("Package is unresolved: " + getPackage().toString());
    return target->getPackage();
}

void DependencyData::setTarget(const ITarget &t)
{
    target = &t;
}

const ITarget &DependencyData::getTarget() const
{
    if (!target)
        throw SW_RUNTIME_ERROR("Package is unresolved: " + getPackage().toString());
    return *target;
}

void NativeCompilerOptionsData::add(const Definition &d)
{
    auto add_def = [this](const String &k, const String &v = {})
    {
        if (v.empty())
            Definitions[k];
        else
            Definitions[k] = v;
    };

    auto p = d.d.find('=');
    if (p == d.d.npos)
    {
        add_def(d.d); // = 1;
        return;
    }
    auto f = d.d.substr(0, p);
    auto s = d.d.substr(p + 1);
    if (s.empty())
        add_def(f + "=");
    else
        add_def(f, s);
}

void NativeCompilerOptionsData::remove(const Definition &d)
{
    auto erase_def = [this](const String &k)
    {
        Definitions.erase(k);
    };

    auto p = d.d.find('=');
    if (p == d.d.npos)
    {
        erase_def(d.d);
        return;
    }
    auto f = d.d.substr(0, p);
    auto s = d.d.substr(p + 1);
    if (s.empty())
        erase_def(f + "=");
    else
        erase_def(f);
}

void NativeCompilerOptionsData::add(const DefinitionsType &defs)
{
    Definitions.insert(defs.begin(), defs.end());
}

void NativeCompilerOptionsData::remove(const DefinitionsType &defs)
{
    for (auto &[k, v] : defs)
        Definitions.erase(k);
}

PathOptionsType NativeCompilerOptionsData::gatherIncludeDirectories() const
{
    PathOptionsType d;
    d.insert(PreIncludeDirectories.begin(), PreIncludeDirectories.end());
    d.insert(IncludeDirectories.begin(), IncludeDirectories.end());
    d.insert(PostIncludeDirectories.begin(), PostIncludeDirectories.end());
    return d;
}

bool NativeCompilerOptionsData::IsIncludeDirectoriesEmpty() const
{
    return PreIncludeDirectories.empty() &&
           IncludeDirectories.empty() &&
           PostIncludeDirectories.empty();
}

void NativeCompilerOptionsData::merge(const NativeCompilerOptionsData &o, const GroupSettings &s)
{
    // report conflicts?

    Definitions.insert(o.Definitions.begin(), o.Definitions.end());
    if (!s.include_directories_only)
    {
        CompileOptions.insert(o.CompileOptions.begin(), o.CompileOptions.end());

        //
        for (auto &[k, v] : o.CustomTargetOptions)
            CustomTargetOptions[k].insert(v.begin(), v.end());
    }

    if (s.merge_to_self)
    {
        unique_merge_containers(PreIncludeDirectories, o.PreIncludeDirectories);
        unique_merge_containers(IncludeDirectories, o.IncludeDirectories);
        unique_merge_containers(PostIncludeDirectories, o.PostIncludeDirectories);
    }
    else
    {
        unique_merge_containers(IncludeDirectories, o.PreIncludeDirectories);
        unique_merge_containers(IncludeDirectories, o.IncludeDirectories);
        unique_merge_containers(IncludeDirectories, o.PostIncludeDirectories);
    }
}

void NativeCompilerOptions::merge(const NativeCompilerOptions &o, const GroupSettings &s)
{
    NativeCompilerOptionsData::merge(o, s);
    System.merge(o.System, s);

    /*if (!s.merge_to_self)
    {
        unique_merge_containers(System.IncludeDirectories, o.PreIncludeDirectories); // merge to System.PreIncludeDirectories?
        unique_merge_containers(System.IncludeDirectories, o.IncludeDirectories);
        unique_merge_containers(System.IncludeDirectories, o.PostIncludeDirectories); // merge to System.PostIncludeDirectories?
    }*/
}

void NativeCompilerOptions::addDefinitions(builder::Command &c) const
{
    auto print_def = [&c](auto &a)
    {
        for (auto &d : a)
        {
            if (d.second.empty())
                c.arguments.push_back("-D" + d.first);
            else
                c.arguments.push_back("-D" + d.first + "=" + d.second);
        }
    };

    print_def(System.Definitions);
    print_def(Definitions);
}

void NativeCompilerOptions::addIncludeDirectories(builder::Command &c, const String &system_idirs_prefix) const
{
    auto print_idir = [&c](const auto &a, auto &flag, int priority)
    {
        for (auto &d : a)
        {
            auto arg = std::make_unique<primitives::command::SimplePositionalArgument>(flag + normalize_path(d));
            arg->getPosition().push_back(priority);
            c.arguments.push_back(std::move(arg));
        }
    };

    constexpr int idir_priority = 100;
    constexpr int sys_idir_priority = 200;

    if (system_idirs_prefix.empty())
    {
        print_idir(NativeCompilerOptionsData::gatherIncludeDirectories(), "-I", idir_priority);
        print_idir(System.gatherIncludeDirectories(), "-I", sys_idir_priority);
    }
    else
    {
        print_idir(NativeCompilerOptionsData::gatherIncludeDirectories(), "-I", idir_priority);
        print_idir(System.gatherIncludeDirectories(), system_idirs_prefix, sys_idir_priority);
    }
}

void NativeCompilerOptions::addDefinitionsAndIncludeDirectories(builder::Command &c, const String &system_idirs_prefix) const
{
    addDefinitions(c);
    addIncludeDirectories(c, system_idirs_prefix);
}

void NativeCompilerOptions::addCompileOptions(builder::Command &c) const
{
    auto print_idir = [&c](const auto &a, auto &flag)
    {
        for (auto &d : a)
            c.arguments.push_back(flag + normalize_path(d));
    };

    print_idir(System.CompileOptions, "");
    print_idir(CompileOptions, "");
}

void NativeCompilerOptions::addEverything(builder::Command &c, const String &system_idirs_prefix) const
{
    addDefinitionsAndIncludeDirectories(c, system_idirs_prefix);
    addCompileOptions(c);
}

PathOptionsType NativeCompilerOptions::gatherIncludeDirectories() const
{
    PathOptionsType idirs;
    auto i = NativeCompilerOptionsData::gatherIncludeDirectories();
    idirs.insert(i.begin(), i.end());
    i = System.gatherIncludeDirectories();
    idirs.insert(i.begin(), i.end());
    return idirs;
}

void NativeLinkerOptionsData::add(const LinkDirectory &l)
{
    LinkDirectories.push_back(l.d);
}

void NativeLinkerOptionsData::remove(const LinkDirectory &l)
{
    LinkDirectories.erase(l.d);
}

void NativeLinkerOptionsData::add(const LinkLibrary &l)
{
    LinkLibraries.push_back(l);
}

void NativeLinkerOptionsData::remove(const LinkLibrary &l)
{
    LinkLibraries.erase(l);
}

PathOptionsType NativeLinkerOptionsData::gatherLinkDirectories() const
{
    PathOptionsType d;
    d.insert(PreLinkDirectories.begin(), PreLinkDirectories.end());
    d.insert(LinkDirectories.begin(), LinkDirectories.end());
    d.insert(PostLinkDirectories.begin(), PostLinkDirectories.end());
    return d;
}

LinkLibrariesType NativeLinkerOptionsData::gatherLinkLibraries() const
{
    return LinkLibraries;
}

bool NativeLinkerOptionsData::IsLinkDirectoriesEmpty() const
{
    return PreLinkDirectories.empty() &&
           LinkDirectories.empty() &&
           PostLinkDirectories.empty();
}

void NativeLinkerOptionsData::merge(const NativeLinkerOptionsData &o, const GroupSettings &s)
{
    // report conflicts?

    unique_merge_containers(Frameworks, o.Frameworks);
    LinkLibraries.insert(o.LinkLibraries.begin(), o.LinkLibraries.end());
    LinkOptions.insert(LinkOptions.end(), o.LinkOptions.begin(), o.LinkOptions.end());
    unique_merge_containers(PreLinkDirectories, o.PreLinkDirectories);
    unique_merge_containers(LinkDirectories, o.LinkDirectories);
    unique_merge_containers(PostLinkDirectories, o.PostLinkDirectories);
    unique_merge_containers(PrecompiledHeaders, o.PrecompiledHeaders);
}

void NativeLinkerOptions::add(const SystemLinkLibrary &l)
{
    System.LinkLibraries.push_back(LinkLibrary{ l.l });
}

void NativeLinkerOptions::remove(const SystemLinkLibrary &l)
{
    System.LinkLibraries.erase(LinkLibrary{ l.l });
}

void NativeLinkerOptions::merge(const NativeLinkerOptions &o, const GroupSettings &s)
{
    // deps are handled separately
    NativeLinkerOptionsData::merge(o, s);
    System.merge(o.System, s);
}

void NativeLinkerOptions::addEverything(builder::Command &c) const
{
    auto print_idir = [&c](const auto &a, auto &flag)
    {
        for (auto &d : a)
            c.arguments.push_back(flag + normalize_path(d));
    };

    print_idir(System.LinkOptions, "");
    print_idir(LinkOptions, "");
}

LinkLibrariesType NativeLinkerOptions::gatherLinkLibraries() const
{
    LinkLibrariesType llib;
    auto i = NativeLinkerOptionsData::gatherLinkLibraries();
    llib.insert(i.begin(), i.end());
    //i = System.gatherLinkLibraries();
    //llib.insert(llib.end(), i.begin(), i.end());
    return llib;
}

DependencyPtr NativeLinkerOptions::operator+(const ITarget &t)
{
    auto d = std::make_shared<Dependency>(t);
    add(d);
    return d;
}

DependencyPtr NativeLinkerOptions::operator+(const DependencyPtr &d)
{
    add(d);
    return d;
}

DependencyPtr NativeLinkerOptions::operator+(const PackageId &pkg)
{
    auto d = std::make_shared<Dependency>(pkg);
    add(d);
    return d;
}

DependencyPtr NativeLinkerOptions::operator+(const UnresolvedPackage &pkg)
{
    auto d = std::make_shared<Dependency>(pkg);
    add(d);
    return d;
}

void NativeLinkerOptions::add(const Target &t)
{
    add(std::make_shared<Dependency>(t));
}

void NativeLinkerOptions::remove(const Target &t)
{
    remove(std::make_shared<Dependency>(t));
}

void NativeLinkerOptions::add(const DependencyPtr &t)
{
    auto i = std::find_if(deps.begin(), deps.end(), [t](const auto &d) {
        return d->getPackage() == t->getPackage();
    });
    if (i == deps.end())
        t->Disabled = false;
    else
        (*i)->Disabled = false;
    deps.push_back(t);

    if (auto t2 = dynamic_cast<TargetOptions *>(this))
        t->settings.mergeMissing(t2->getTarget().getExportOptions()); // add only missing fields!
}

void NativeLinkerOptions::remove(const DependencyPtr &t)
{
    t->Disabled = true;
    for (auto &d : deps)
    {
        if (d->getPackage() != t->getPackage())
            continue;
        d->Disabled = true;
    }
    deps.push_back(t);

    if (auto t2 = dynamic_cast<TargetOptions *>(this))
        t->settings.mergeMissing(t2->getTarget().getExportOptions()); // add only missing fields!f
}

void NativeLinkerOptions::add(const UnresolvedPackage &t)
{
    add(std::make_shared<Dependency>(t));
}

void NativeLinkerOptions::remove(const UnresolvedPackage &t)
{
    remove(std::make_shared<Dependency>(t));
}

void NativeLinkerOptions::add(const UnresolvedPackages &t)
{
    for (auto &d : t)
        add(d);
}

void NativeLinkerOptions::remove(const UnresolvedPackages &t)
{
    for (auto &d : t)
        remove(d);
}

void NativeLinkerOptions::add(const PackageId &p)
{
    add(std::make_shared<Dependency>(p));
}

void NativeLinkerOptions::remove(const PackageId &p)
{
    remove(std::make_shared<Dependency>(p));
}

void NativeOptions::merge(const NativeOptions &o, const GroupSettings &s)
{
    NativeCompilerOptions::merge(o, s);
    NativeLinkerOptions::merge(o, s);
}

} // namespace sw
