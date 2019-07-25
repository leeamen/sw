// Copyright (C) 2017-2018 Egor Pugin <egor.pugin@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "sw/core/sw_context.h"

#include "misc/cmVSSetupHelper.h"

#include <sw/builder/program.h>
#include <sw/builder/program_version_storage.h>

#include <boost/algorithm/string.hpp>

#include <regex>
#include <string>

#include <primitives/log.h>
DECLARE_STATIC_LOGGER(logger, "compiler.detect");

#define DETECT_ARGS SwCoreContext &s

namespace sw
{

const StringSet &getCppHeaderFileExtensions()
{
    static const StringSet header_file_extensions{
        ".h",
        ".hh",
        ".hm",
        ".hpp",
        ".hxx",
        ".tcc",
        ".h++",
        ".H++",
        ".HPP",
        ".H",
    };
    return header_file_extensions;
}

const StringSet &getCppSourceFileExtensions()
{
    static const StringSet cpp_source_file_extensions{
        ".cc",
        ".CC",
        ".cpp",
        ".cp",
        ".cxx",
        //".ixx", // msvc modules?
        // cppm - clang?
        // mxx, mpp - build2?
        ".c++",
        ".C++",
        ".CPP",
        ".CXX",
        ".C", // old ext (Wt)
        // Objective-C
        ".m",
        ".mm",
    };
    return cpp_source_file_extensions;
}

std::string getVsToolset(const Version &v);

void detectNativeCompilers(DETECT_ARGS);
void detectCSharpCompilers(DETECT_ARGS);
void detectRustCompilers(DETECT_ARGS);
void detectGoCompilers(DETECT_ARGS);
void detectFortranCompilers(DETECT_ARGS);
void detectJavaCompilers(DETECT_ARGS);
void detectKotlinCompilers(DETECT_ARGS);
void detectDCompilers(DETECT_ARGS);

bool isCppHeaderFileExtension(const String &e)
{
    auto &exts = getCppHeaderFileExtensions();
    return exts.find(e) != exts.end();
}

bool isCppSourceFileExtensions(const String &e)
{
    auto &exts = getCppSourceFileExtensions();
    return exts.find(e) != exts.end();
}

void detectCompilers(DETECT_ARGS)
{
    detectNativeCompilers(s);

    // others
    /*detectCSharpCompilers(s);
    detectRustCompilers(s);
    detectGoCompilers(s);
    detectFortranCompilers(s);
    detectJavaCompilers(s);
    detectKotlinCompilers(s);
    detectDCompilers(s);*/
}

// left join comparator

struct PredefinedTarget : ITarget
{
    PackageId id;
    TargetSettings ts;
    TargetSettings public_ts;

    PredefinedTarget(const PackageId &id) :id(id) {}
    virtual ~PredefinedTarget() {}

    const PackageId &getPackage() const override { return id; }

    const Source &getSource() const override { static EmptySource es; return es; }
    Files getSourceFiles() const override { SW_UNIMPLEMENTED; }
    std::vector<IDependency *> getDependencies() const override { return {}; }
    bool prepare() override { return false; }
    Commands getCommands() const override { return {}; }

    const TargetSettings &getInterfaceSettings() const override { return public_ts; }

    bool operator==(const TargetSettings &s) const override { return ts == s; }
    //bool operator<(const TargetSettings &s) const override { return ts < s; }
};

struct PredefinedProgramTarget : PredefinedTarget, PredefinedProgram
{
    using PredefinedTarget::PredefinedTarget;
};

template <class T>
static T &addTarget(SwCoreContext &s, const PackageId &id)
{
    LOG_TRACE(logger, "Detected target: " + id.toString());

    auto t = std::make_shared<T>(id);

    auto &cld = s.getPredefinedTargets();
    cld[id].push_back(t);

    //t.sw_provided = true;
    return *t;
}

template <class T = PredefinedProgramTarget>
static T &addProgram(SwCoreContext &s, const PackageId &id, const std::shared_ptr<Program> &p)
{
    auto &t = addTarget<T>(s, id);
    t.setProgram(p);
    LOG_TRACE(logger, "Detected program: " + p->file.u8string());
    return t;
}

void detectDCompilers(DETECT_ARGS)
{
    SW_UNIMPLEMENTED;

    /*path compiler;
    compiler = resolveExecutable("dmd");
    if (compiler.empty())
        return;

    auto C = std::make_shared<DCompiler>(s.swctx);
    C->file = compiler;
    C->Extension = s.getBuildSettings().TargetOS.getExecutableExtension();
    //C->input_extensions = { ".d" };
    addProgram(s, "org.dlang.dmd.dmd", C);*/
}

void detectKotlinCompilers(DETECT_ARGS)
{
    SW_UNIMPLEMENTED;

    /*path compiler;
    compiler = resolveExecutable("kotlinc");
    if (compiler.empty())
        return;

    auto C = std::make_shared<KotlinCompiler>(s.swctx);
    C->file = compiler;
    //C->input_extensions = { ".kt", ".kts" };
    //s.registerProgram("com.JetBrains.kotlin.kotlinc", C);*/
}

void detectJavaCompilers(DETECT_ARGS)
{
    SW_UNIMPLEMENTED;

    /*path compiler;
    compiler = resolveExecutable("javac");
    if (compiler.empty())
        return;
    //compiler = resolveExecutable("jar"); // later

    auto C = std::make_shared<JavaCompiler>(s.swctx);
    C->file = compiler;
    //C->input_extensions = { ".java", };
    //s.registerProgram("com.oracle.java.javac", C);*/
}

void detectFortranCompilers(DETECT_ARGS)
{
    SW_UNIMPLEMENTED;

    /*path compiler;
    compiler = resolveExecutable("gfortran");
    if (compiler.empty())
    {
        compiler = resolveExecutable("f95");
        if (compiler.empty())
        {
            compiler = resolveExecutable("g95");
            if (compiler.empty())
            {
                return;
            }
        }
    }

    auto C = std::make_shared<FortranCompiler>(s.swctx);
    C->file = compiler;
    SW_UNIMPLEMENTED;
    //C->Extension = s.Settings.TargetOS.getExecutableExtension();
    /*C->input_extensions = {
        ".f",
        ".FOR",
        ".for",
        ".f77",
        ".f90",
        ".f95",

        // support Preprocessing
        ".F",
        ".fpp",
        ".FPP",
    };*/
    //s.registerProgram("org.gnu.gcc.fortran", C);*/
}

void detectGoCompilers(DETECT_ARGS)
{
    SW_UNIMPLEMENTED;

#if defined(_WIN32)
    /*auto compiler = path("go");
    compiler = resolveExecutable(compiler);
    if (compiler.empty())
        return;

    auto C = std::make_shared<GoCompiler>(s.swctx);
    C->file = compiler;
    SW_UNIMPLEMENTED;
    //C->Extension = s.Settings.TargetOS.getExecutableExtension();
    //C->input_extensions = { ".go", };
    //s.registerProgram("org.google.golang.go", C);*/
#else
#endif
}

void detectRustCompilers(DETECT_ARGS)
{
    SW_UNIMPLEMENTED;

#if defined(_WIN32)
    /*auto compiler = get_home_directory() / ".cargo" / "bin" / "rustc";
    compiler = resolveExecutable(compiler);
    if (compiler.empty())
        return;

    auto C = std::make_shared<RustCompiler>(s.swctx);
    C->file = compiler;
    SW_UNIMPLEMENTED;
    //C->Extension = s.Settings.TargetOS.getExecutableExtension();
    //C->input_extensions = { ".rs", };
    //s.registerProgram("org.rust.rustc", C);*/
#else
#endif
}

struct VSInstance
{
    path root;
    Version version;
};

using VSInstances = VersionMap<VSInstance>;

struct SimpleProgram : Program
{
    using Program::Program;

    std::shared_ptr<Program> clone() const override { return std::make_shared<SimpleProgram>(*this); }
    std::shared_ptr<builder::Command> getCommand() const override
    {
        if (!cmd)
        {
            cmd = std::make_shared<builder::Command>(swctx);
            cmd->setProgram(file);
        }
        return cmd;
    }

private:
    mutable std::shared_ptr<builder::Command> cmd;
};

VSInstances &gatherVSInstances(DETECT_ARGS)
{
    static VSInstances instances = [&s]()
    {
        VSInstances instances;
#ifdef _WIN32
        cmVSSetupAPIHelper h;
        h.EnumerateVSInstances();
        for (auto &i : h.instances)
        {
            path root = i.VSInstallLocation;
            Version v = to_string(i.Version);

            // actually, it does not affect cl.exe or other tool versions
            if (i.VSInstallLocation.find(L"Preview") != std::wstring::npos)
                v = v.toString() + "-preview";

            VSInstance inst;
            inst.root = root;
            inst.version = v;
            instances.emplace(v, inst);
        }
#endif
        return instances;
    }();
    return instances;
}

void detectCSharpCompilers(DETECT_ARGS)
{
    SW_UNIMPLEMENTED;

    /*auto &instances = gatherVSInstances(s);
    for (auto &[v, i] : instances)
    {
        auto root = i.root;
        switch (v.getMajor())
        {
        case 15:
            root = root / "MSBuild" / "15.0" / "Bin" / "Roslyn";
            break;
        case 16:
            root = root / "MSBuild" / "Current" / "Bin" / "Roslyn";
            break;
        default:
            SW_UNIMPLEMENTED;
        }

        auto compiler = root / "csc.exe";

        auto C = std::make_shared<VisualStudioCSharpCompiler>(s.swctx);
        C->file = compiler;
        SW_UNIMPLEMENTED;
        //C->Extension = s.Settings.TargetOS.getExecutableExtension();
        //C->input_extensions = { ".cs", };
        //s.registerProgram("com.Microsoft.VisualStudio.Roslyn.csc", C);
    }*/
}

void detectMsvc15Plus(DETECT_ARGS)
{
    // https://docs.microsoft.com/en-us/cpp/c-runtime-library/crt-library-features?view=vs-2019

    auto &instances = gatherVSInstances(s);
    const auto host = toStringWindows(s.getHostOs().Arch);
    auto new_settings = s.getHostOs();

    for (auto target_arch : {ArchType::x86_64,ArchType::x86,ArchType::arm,ArchType::aarch64})
    {
        new_settings.Arch = target_arch;

        auto ts1 = toTargetSettings(new_settings);
        TargetSettings ts;
        ts["os"]["kernel"] = ts1["os"]["kernel"];
        ts["os"]["arch"] = ts1["os"]["arch"];

        for (auto &[_, instance] : instances)
        {
            auto root = instance.root / "VC";
            auto v = instance.version;

            if (v.getMajor() < 15)
            {
                // continue; // instead of throw ?
                throw SW_RUNTIME_ERROR("VS < 15 must be handled differently");
            }

            root = root / "Tools" / "MSVC" / boost::trim_copy(read_file(root / "Auxiliary" / "Build" / "Microsoft.VCToolsVersion.default.txt"));

            // get suffix
            auto target = toStringWindows(target_arch);

            auto compiler = root / "bin";
            auto host_root = compiler / ("Host" + host) / host;

            compiler /= path("Host" + host) / target;

            // VS programs inherit cl.exe version (V)
            // same for VS libs
            // because ml,ml64,lib,link version (O) has O.Major = V.Major - 5
            // e.g., V = 19.21..., O = 14.21.... (19 - 5 = 14)

            // C, C++
            {
                auto p = std::make_shared<SimpleProgram>(s);
                p->file = compiler / "cl.exe";
                if (fs::exists(p->file))
                {
                    auto c = p->getCommand();
                    if (s.getHostOs().Arch != target_arch)
                        c->addPathDirectory(host_root);
                    // run getVersion via prepared command
                    builder::detail::ResolvableCommand c2 = *c;
                    v = getVersion(s, c2);
                    if (instance.version.isPreRelease())
                        v.getExtra() = instance.version.getExtra();
                    auto &cl = addProgram(s, PackageId("com.Microsoft.VisualStudio.VC.cl", v), p);
                    cl.ts = ts;
                }
            }

            // lib, link
            {
                auto p = std::make_shared<SimpleProgram>(s);
                p->file = compiler / "link.exe";
                if (fs::exists(p->file))
                {
                    auto &link = addProgram(s, PackageId("com.Microsoft.VisualStudio.VC.link", v), p);
                    link.ts = ts;
                }

                if (s.getHostOs().Arch != target_arch)
                {
                    auto c = p->getCommand();
                    c->addPathDirectory(host_root);
                }

                p = std::make_shared<SimpleProgram>(s);
                p->file = compiler / "lib.exe";
                if (fs::exists(p->file))
                {
                    auto &lib = addProgram(s, PackageId("com.Microsoft.VisualStudio.VC.lib", v), p);
                    lib.ts = ts;
                }

                if (s.getHostOs().Arch != target_arch)
                {
                    auto c = p->getCommand();
                    c->addPathDirectory(host_root);
                }
            }

            // ASM
            if (target_arch == ArchType::x86_64 || target_arch == ArchType::x86)
            {
                auto p = std::make_shared<SimpleProgram>(s);
                p->file = compiler / (target_arch == ArchType::x86_64 ? "ml64.exe" : "ml.exe");
                if (fs::exists(p->file))
                {
                    auto &ml = addProgram(s, PackageId("com.Microsoft.VisualStudio.VC.ml", v), p);
                    ml.ts = ts;
                }
            }

            // libc++
            {
                auto &libcpp = addTarget<PredefinedTarget>(s, PackageId("com.Microsoft.VisualStudio.VC.libcpp", v));
                libcpp.ts = ts;
                libcpp.public_ts["system-include-directories"].push_back(normalize_path(root / "include"));
                libcpp.public_ts["system-link-directories"].push_back(normalize_path(root / "lib" / target));

                if (fs::exists(root / "ATLMFC" / "include"))
                {
                    auto &atlmfc = addTarget<PredefinedTarget>(s, PackageId("com.Microsoft.VisualStudio.VC.ATLMFC", v));
                    atlmfc.ts = ts;
                    atlmfc.public_ts["system-include-directories"].push_back(normalize_path(root / "ATLMFC" / "include"));
                    atlmfc.public_ts["system-link-directories"].push_back(normalize_path(root / "ATLMFC" / "lib" / target));
                }
            }
        }
    }
}

void detectMsvc14AndOlder(DETECT_ARGS)
{
    auto find_comn_tools = [](const Version &v) -> path
    {
        auto n = std::to_string(v.getMajor()) + std::to_string(v.getMinor());
        auto ver = "VS"s + n + "COMNTOOLS";
        auto e = getenv(ver.c_str());
        if (e)
        {
            String r = e;
            while (!r.empty() && (r.back() == '/' || r.back() == '\\'))
                r.pop_back();
            path root = r;
            root = root.parent_path().parent_path();
            return root;
        }
        return {};
    };

    auto toStringWindows14AndOlder = [](ArchType e)
    {
        switch (e)
        {
        case ArchType::x86_64:
            return "amd64";
        case ArchType::x86:
            return "x86";
        case ArchType::arm:
            return "arm";
        default:
            throw SW_RUNTIME_ERROR("Unknown Windows arch");
        }
    };

    auto new_settings = s.getHostOs();

    // no ArchType::aarch64?
    for (auto target_arch : { ArchType::x86_64,ArchType::x86,ArchType::arm, })
    {
        // following code is written using VS2015
        // older versions might need special handling

        new_settings.Arch = target_arch;

        auto ts1 = toTargetSettings(new_settings);
        TargetSettings ts;
        ts["os"]["kernel"] = ts1["os"]["kernel"];
        ts["os"]["arch"] = ts1["os"]["arch"];

        for (auto n : {14,12,11,10,9,8})
        {
            Version v(n);
            auto root = find_comn_tools(v);
            if (root.empty())
                continue;

            root /= "VC";

            // get suffix
            auto target = toStringWindows14AndOlder(target_arch);

            auto compiler = root / "bin";
            auto host_root = compiler;

            path libdir = "lib";
            libdir /= toStringWindows14AndOlder(target_arch);

            // VC/bin/ ... x86 files
            // VC/bin/amd64/ ... x86_64 files
            // VC/bin/arm/ ... arm files
            // so we need to add subdir for non x86 targets
            if (!s.getHostOs().is(ArchType::x86))
            {
                host_root /= toStringWindows14AndOlder(s.getHostOs().Arch);
            }

            // now set to root
            compiler = host_root;

            // VC/bin/x86_amd64
            // VC/bin/x86_arm
            // VC/bin/amd64_x86
            // VC/bin/amd64_arm
            if (s.getHostOs().Arch != target_arch)
            {
                //if (!s.getHostOs().is(ArchType::x86))
                compiler += "_"s + toStringWindows14AndOlder(target_arch);
            }

            // VS programs inherit cl.exe version (V)
            // same for VS libs
            // because ml,ml64,lib,link version (O) has O.Major = V.Major - 5
            // e.g., V = 19.21..., O = 14.21.... (19 - 5 = 14)

            // C, C++
            {
                auto p = std::make_shared<SimpleProgram>(s);
                p->file = compiler / "cl.exe";
                if (fs::exists(p->file))
                {
                    auto c = p->getCommand();
                    if (s.getHostOs().Arch != target_arch)
                        c->addPathDirectory(host_root);
                    // run getVersion via prepared command
                    builder::detail::ResolvableCommand c2 = *c;
                    v = getVersion(s, c2);
                    auto &cl = addProgram(s, PackageId("com.Microsoft.VisualStudio.VC.cl", v), p);
                    cl.ts = ts;
                }
                else
                    continue;
            }

            // lib, link
            {
                auto p = std::make_shared<SimpleProgram>(s);
                p->file = compiler / "link.exe";
                if (fs::exists(p->file))
                {
                    auto &link = addProgram(s, PackageId("com.Microsoft.VisualStudio.VC.link", v), p);
                    link.ts = ts;
                }

                if (s.getHostOs().Arch != target_arch)
                {
                    auto c = p->getCommand();
                    c->addPathDirectory(host_root);
                }

                p = std::make_shared<SimpleProgram>(s);
                p->file = compiler / "lib.exe";
                if (fs::exists(p->file))
                {
                    auto &lib = addProgram(s, PackageId("com.Microsoft.VisualStudio.VC.lib", v), p);
                    lib.ts = ts;
                }

                if (s.getHostOs().Arch != target_arch)
                {
                    auto c = p->getCommand();
                    c->addPathDirectory(host_root);
                }
            }

            // ASM
            if (target_arch == ArchType::x86_64 || target_arch == ArchType::x86)
            {
                auto p = std::make_shared<SimpleProgram>(s);
                p->file = compiler / (target_arch == ArchType::x86_64 ? "ml64.exe" : "ml.exe");
                if (fs::exists(p->file))
                {
                    auto &ml = addProgram(s, PackageId("com.Microsoft.VisualStudio.VC.ml", v), p);
                    ml.ts = ts;
                }
            }

            // libc++
            {
                auto &libcpp = addTarget<PredefinedTarget>(s, PackageId("com.Microsoft.VisualStudio.VC.libcpp", v));
                libcpp.ts = ts;
                libcpp.public_ts["system-include-directories"].push_back(normalize_path(root / "include"));
                libcpp.public_ts["system-link-directories"].push_back(normalize_path(root / libdir));

                if (fs::exists(root / "ATLMFC" / "include"))
                {
                    auto &atlmfc = addTarget<PredefinedTarget>(s, PackageId("com.Microsoft.VisualStudio.VC.ATLMFC", v));
                    atlmfc.ts = ts;
                    atlmfc.public_ts["system-include-directories"].push_back(normalize_path(root / "ATLMFC" / "include"));
                    atlmfc.public_ts["system-link-directories"].push_back(normalize_path(root / "ATLMFC" / libdir));
                }
            }
        }
    }
}

static path getProgramFilesX86()
{
    auto e = getenv("programfiles(x86)");
    if (!e)
        throw SW_RUNTIME_ERROR("Cannot get 'programfiles(x86)' env. var.");
    return e;
}

static path getWindowsKitRoot()
{
    // take from registry?
    auto p = getProgramFilesX86() / "Windows Kits";
    if (fs::exists(p))
        return p;
    throw SW_RUNTIME_ERROR("No Windows Kits available");
}

static String getWin10KitDirName()
{
    return "10";
}

static Strings listWindowsKits()
{
    // https://en.wikipedia.org/wiki/Microsoft_Windows_SDK
    static const Strings known_kits{ getWin10KitDirName(), "8.1A", "8.1", "8.0", "7.1A", "7.1", "7.0A", "7.0A","6.0A" };

    Strings kits;
    auto kr = getWindowsKitRoot();
    for (auto &k : known_kits)
    {
        auto d = kr / k;
        if (fs::exists(d))
            kits.push_back(k);
    }
    return kits;
}

static path getWin10KitInspectionDir()
{
    auto kr = getWindowsKitRoot();
    auto dir = kr / getWin10KitDirName() / "Include";
    return dir;
}

static VersionSet listWindows10Kits()
{
    VersionSet kits;
    auto dir = getWin10KitInspectionDir();
    for (auto &i : fs::directory_iterator(dir))
    {
        if (fs::is_directory(i))
        {
            auto d = i.path().filename().u8string();
            Version v = d;
            if (v.isVersion())
                kits.insert(d);
        }
    }
    return kits;
}

void detectWindowsSdk(DETECT_ARGS)
{
    // ucrt - universal CRT
    //
    // um - user mode
    // km - kernel mode
    // shared - some of these and some of these
    //

    auto new_settings = s.getHostOs();

    struct WinKit
    {
        path kit_root;

        String name;

        String bdir_subversion;
        String idir_subversion;
        String ldir_subversion;

        Strings idirs; // additional idirs

        void add(SwCoreContext &s, OS &new_settings, const Version &v)
        {
            auto idir = kit_root / "Include" / idir_subversion;
            if (!fs::exists(idir / name))
                return;

            for (auto target_arch : { ArchType::x86_64,ArchType::x86,ArchType::arm,ArchType::aarch64 })
            {
                new_settings.Arch = target_arch;

                auto ts1 = toTargetSettings(new_settings);
                TargetSettings ts;
                ts["os"]["kernel"] = ts1["os"]["kernel"];
                ts["os"]["arch"] = ts1["os"]["arch"];

                auto libdir = kit_root / "Lib" / ldir_subversion / name / toStringWindows(target_arch);
                if (fs::exists(libdir))
                {
                    auto &ucrt = addTarget<PredefinedTarget>(s, PackageId("com.Microsoft.Windows.SDK." + name, v));
                    ucrt.ts = ts;
                    ucrt.ts["os"]["version"] = v.toString(3); // use 3 numbers at the moment

                    ucrt.public_ts["system-include-directories"].push_back(normalize_path(idir / name));
                    for (auto &i : idirs)
                        ucrt.public_ts["system-include-directories"].push_back(normalize_path(idir / i));
                    ucrt.public_ts["system-link-directories"].push_back(normalize_path(libdir));
                }
            }
        }

        void addTools(SwCoreContext &s, OS &new_settings)
        {
            // .rc
            {
                auto p = std::make_shared<SimpleProgram>(s);
                p->file = kit_root / "bin" / bdir_subversion / toStringWindows(s.getHostOs().Arch) / "rc.exe";
                if (fs::exists(p->file))
                {
                    Version v = getVersion(s, p->file, "/?");
                    auto &rc = addProgram(s, PackageId("com.Microsoft.Windows.rc", v), p);
                    auto ts1 = toTargetSettings(new_settings);
                    rc.ts["os"]["kernel"] = ts1["os"]["kernel"];
                }
                // these are passed from compiler during merge?
                //for (auto &idir : COpts.System.IncludeDirectories)
                //C->system_idirs.push_back(idir);
            }
        }
    };

    for (auto &k : listWindowsKits())
    {
        auto kr = getWindowsKitRoot() /k;
        if (k == getWin10KitDirName())
        {
            for (auto &v : listWindows10Kits())
            {
                // ucrt
                {
                    WinKit wk;
                    wk.name = "ucrt";
                    wk.kit_root = kr;
                    wk.idir_subversion = v.toString();
                    wk.ldir_subversion = v.toString();
                    wk.add(s, new_settings, v);
                }

                // um + shared
                {
                    WinKit wk;
                    wk.name = "um";
                    wk.kit_root = kr;
                    wk.idir_subversion = v.toString();
                    wk.ldir_subversion = v.toString();
                    wk.idirs.push_back("shared");
                    wk.add(s, new_settings, v);
                }

                // km
                {
                    WinKit wk;
                    wk.name = "km";
                    wk.kit_root = kr;
                    wk.idir_subversion = v.toString();
                    wk.ldir_subversion = v.toString();
                    wk.add(s, new_settings, v);
                }

                // tools
                {
                    WinKit wk;
                    wk.kit_root = kr;
                    wk.bdir_subversion = v.toString();
                    wk.addTools(s, new_settings);
                }
            }
        }
        else
        {
            // um + shared
            {
                WinKit wk;
                wk.name = "um";
                wk.kit_root = kr;
                if (k == "8.1")
                    wk.ldir_subversion = "winv6.3";
                else
                    LOG_DEBUG(logger, "TODO: Windows Kit " + k + " is not implemented yet. Report this issue.");
                wk.idirs.push_back("shared");
                wk.add(s, new_settings, k);
            }

            // km
            {
                WinKit wk;
                wk.name = "km";
                wk.kit_root = kr;
                if (k == "8.1")
                    wk.ldir_subversion = "winv6.3";
                else
                    LOG_DEBUG(logger, "TODO: Windows Kit " + k + " is not implemented yet. Report this issue.");
                wk.add(s, new_settings, k);
            }

            // tools
            {
                WinKit wk;
                wk.kit_root = kr;
                wk.addTools(s, new_settings);
            }
        }
    }
}

void detectMsvc(DETECT_ARGS)
{
    detectMsvc15Plus(s);
    detectMsvc14AndOlder(s);
    detectWindowsSdk(s);
}

void detectWindowsClang(DETECT_ARGS)
{
    // create programs
    const path base_llvm_path = path("c:") / "Program Files" / "LLVM";
    const path bin_llvm_path = base_llvm_path / "bin";

    // clang-cl, move to msvc?

    // C, C++
    {
        auto p = std::make_shared<SimpleProgram>(s);
        p->file = bin_llvm_path / "clang-cl.exe";
        //C->file = base_llvm_path / "msbuild-bin" / "cl.exe";
        // clangcl is able to find VC STL itself
        // also we could provide command line arg -fms-compat...=19.16 19.20 or smth like that
        //COpts2.System.IncludeDirectories.insert(bin_llvm_path / "lib" / "clang" / C->getVersion().toString() / "include");
        //COpts2.System.CompileOptions.push_back("-Wno-everything");
        if (!fs::exists(p->file))
        {
            auto f = resolveExecutable("clang-cl");
            if (fs::exists(f))
                p->file = f;
        }
        if (fs::exists(p->file))
        {
            Version v = getVersion(s, p->file);
            auto &c = addProgram(s, PackageId("org.LLVM.clangcl", v), p);
        }
    }

    // clang

    // link
    {
        auto p = std::make_shared<SimpleProgram>(s);
        p->file = bin_llvm_path / "lld.exe";
        if (!fs::exists(p->file))
        {
            auto f = resolveExecutable("lld");
            if (fs::exists(f))
                p->file = f;
        }
        if (fs::exists(p->file))
        {
            Version v = getVersion(s, p->file);
            auto &c = addProgram(s, PackageId("org.LLVM.lld", v), p);
        }
    }

    // ar
    {
        auto p = std::make_shared<SimpleProgram>(s);
        p->file = bin_llvm_path / "llvm-ar.exe";
        if (!fs::exists(p->file))
        {
            auto f = resolveExecutable("llvm-ar");
            if (fs::exists(f))
                p->file = f;
        }
        if (fs::exists(p->file))
        {
            Version v = getVersion(s, p->file);
            auto &c = addProgram(s, PackageId("org.LLVM.ar", v), p);
        }
    }

    // C
    {
        auto p = std::make_shared<SimpleProgram>(s);
        p->file = bin_llvm_path / "clang.exe";
        if (!fs::exists(p->file))
        {
            auto f = resolveExecutable("clang");
            if (fs::exists(f))
                p->file = f;
        }
        if (fs::exists(p->file))
        {
            Version v = getVersion(s, p->file);
            auto &c = addProgram(s, PackageId("org.LLVM.clang", v), p);
        }
        auto c = p->getCommand();
        c->push_back("-Wno-everything");
        // is it able to find VC STL itself?
        //COpts2.System.IncludeDirectories.insert(base_llvm_path / "lib" / "clang" / C->getVersion().toString() / "include");
    }

    // C++
    {
        auto p = std::make_shared<SimpleProgram>(s);
        p->file = bin_llvm_path / "clang++.exe";
        if (!fs::exists(p->file))
        {
            auto f = resolveExecutable("clang++");
            if (fs::exists(f))
                p->file = f;
        }
        if (fs::exists(p->file))
        {
            Version v = getVersion(s, p->file);
            auto &c = addProgram(s, PackageId("org.LLVM.clangpp", v), p);
        }
        auto c = p->getCommand();
        c->push_back("-Wno-everything");
        // is it able to find VC STL itself?
        //COpts2.System.IncludeDirectories.insert(base_llvm_path / "lib" / "clang" / C->getVersion().toString() / "include");
    }
}

void detectWindowsCompilers(DETECT_ARGS)
{
    detectMsvc(s);
    detectWindowsClang(s);
}

void detectNonWindowsCompilers(DETECT_ARGS)
{
    path p;

    SW_UNIMPLEMENTED;

    /*NativeLinkerOptions LOpts;

    //LOpts.System.LinkLibraries.push_back("pthread"); // remove and add to progs explicitly?
    //LOpts.System.LinkLibraries.push_back("dl"); // remove and add to progs explicitly?
    //LOpts.System.LinkLibraries.push_back("m"); // remove and add to progs explicitly?

    auto resolve = [](const path &p)
    {
        //if (do_not_resolve_compiler)
            //return p;
        return resolveExecutable(p);
    };

    p = resolve("ar");
    if (!p.empty())
    {
        auto Librarian = std::make_shared<GNULibrarian>(s.swctx);
        Librarian->Type = LinkerType::GNU;
        Librarian->file = p;
        SW_UNIMPLEMENTED;
        //Librarian->Prefix = s.Settings.TargetOS.getLibraryPrefix();
        //Librarian->Extension = s.Settings.TargetOS.getStaticLibraryExtension();
        *Librarian = LOpts;
        //s.registerProgram("org.gnu.binutils.ar", Librarian);
        if (s.getHostOs().is(OSType::Macos))
            Librarian->createCommand(s.swctx)->use_response_files = false;
    }

    FilesOrdered gcc_vers{ "gcc" };
    FilesOrdered gccpp_vers{ "g++" };
    for (int i = 4; i < 12; i++)
    {
        gcc_vers.push_back(path(gcc_vers[0]) += "-" + std::to_string(i));
        gccpp_vers.push_back(path(gccpp_vers[0]) += "-" + std::to_string(i));
    }
    FilesOrdered clang_vers{ "clang" };
    FilesOrdered clangpp_vers{ "clang++" };
    for (int i = 3; i < 16; i++)
    {
        clang_vers.push_back(path(clang_vers[0]) += "-" + std::to_string(i));
        clangpp_vers.push_back(path(clangpp_vers[0]) += "-" + std::to_string(i));
    }
    if (s.getHostOs().is(OSType::Macos))
    {
        // also detect brew
        if (fs::exists("/usr/local/Cellar/llvm"))
        for (auto &d : fs::directory_iterator("/usr/local/Cellar/llvm"))
        {
            clang_vers.push_back(d.path() / "bin/clang");
            clangpp_vers.push_back(d.path() / "bin/clang++");
        }
    }*/

    //p = resolve("ld.gold");
    //for (auto &v : gcc_vers)
    //for (auto &v : gccpp_vers) // this links correct c++ library
    {
        SW_UNIMPLEMENTED;

        /*p = resolve(v);
        if (!p.empty())
        {
            auto Linker = std::make_shared<GNULinker>(s.swctx);

            if (s.getHostOs().is(OSType::Macos))
                Linker->use_start_end_groups = false;
            Linker->Type = LinkerType::GNU;
            Linker->file = p;
            SW_UNIMPLEMENTED;
            //Linker->Prefix = s.Settings.TargetOS.getLibraryPrefix();
            //Linker->Extension = s.Settings.TargetOS.getStaticLibraryExtension();

            auto lopts2 = LOpts;
            //lopts2.System.LinkLibraries.push_back("stdc++"); // remove and add to progs explicitly?
            //lopts2.System.LinkLibraries.push_back("stdc++fs"); // remove and add to progs explicitly?

            *Linker = lopts2;
            //s.registerProgram("org.gnu.gcc.ld", Linker);
        }*/
    }

    //NativeCompilerOptions COpts;

    path macos_sdk_dir;
    SW_UNIMPLEMENTED;
    //if (s.Settings.TargetOS.is(OSType::Macos) || s.Settings.TargetOS.is(OSType::IOS))
        //macos_sdk_dir = s.Settings.Native.SDK.getPath();

    auto is_apple_clang = [](const path &p)
    {
        primitives::Command c;
        c.setProgram(p);
        c.arguments.push_back("--version");
        error_code ec;
        c.execute(ec);
        if (ec)
        {
            LOG_TRACE(logger, "is_apple_clang: not resolved: " + p.u8string());
            return false;
        }
        return c.out.text.find("Apple") != String::npos;
    };

    // ASM
    /*{
        p = resolve("as");

        auto L = std::make_shared<NativeLanguage>();
        //L->Type = LanguageType::ASM;
        // .s - pure asm
        // .S - with #define (accepts -D) and #include (accepts -I), also .sx
        L->CompiledExtensions = { ".s", ".S", ".sx" };
        //s.registerLanguage(L);

        //auto L = (ASMLanguage*)s.languages[LanguageType::ASM].get();
        auto C = std::make_shared<GNUASMCompiler>();
        C->Type = CompilerType::GNU;
        C->file = p;
        *C = COpts;
        L->compiler = C;
        s.registerProgram("org.gnu.gcc.as", C);
    }*/

    /*for (auto &v : gcc_vers)
    {
        p = resolve(v);
        if (!p.empty())
        {
            SW_UNIMPLEMENTED;

            // C
            {
                auto C = std::make_shared<GNUCompiler>(s.swctx);
                C->Type = CompilerType::GNU;
                C->file = p;
                *C = COpts;
                // also with asm
                // .s - pure asm
                // .S - with #define (accepts -D) and #include (accepts -I), also .sx
                //C->input_extensions = { ".c", ".s", ".S" };
                //s.registerProgram("org.gnu.gcc.gcc", C);

                if (!macos_sdk_dir.empty())
                    C->IncludeSystemRoot = macos_sdk_dir;
            }
        }
    }

    for (auto &v : gccpp_vers)
    {
        p = resolve(v);
        if (!p.empty())
        {
            SW_UNIMPLEMENTED;

            // CPP
            {
                auto C = std::make_shared<GNUCompiler>(s.swctx);
                C->Type = CompilerType::GNU;
                C->file = p;
                *C = COpts;
                //C->input_extensions = getCppSourceFileExtensions();
                //s.registerProgram("org.gnu.gcc.gpp", C);

                if (!macos_sdk_dir.empty())
                    C->IncludeSystemRoot = macos_sdk_dir;
            }
        }
    }*/

    // llvm/clang
    {
        SW_UNIMPLEMENTED;
        //p = resolve("llvm-ar");
        if (!p.empty())
        {
            SW_UNIMPLEMENTED;

            /*auto Librarian = std::make_shared<GNULibrarian>(s.swctx);
            Librarian->Type = LinkerType::GNU;
            Librarian->file = p;
            SW_UNIMPLEMENTED;
            //Librarian->Prefix = s.Settings.TargetOS.getLibraryPrefix();
            //Librarian->Extension = s.Settings.TargetOS.getStaticLibraryExtension();
            *Librarian = LOpts;
            //s.registerProgram("org.LLVM.ar", Librarian);
            //if (s.getHostOs().is(OSType::Macos))
            //Librarian->createCommand()->use_response_files = false;*/
        }

        ////p = resolve("ld.gold");
        SW_UNIMPLEMENTED;
        /*for (auto &v : clang_vers)
        //for (auto &v : clangpp_vers) // this links correct c++ library
        {
            p = resolve(v);
            if (!p.empty())
            {
                SW_UNIMPLEMENTED;

                bool appleclang = is_apple_clang(p);

                auto Linker = std::make_shared<GNULinker>(s.swctx);

                if (s.getHostOs().is(OSType::Macos))
                    Linker->use_start_end_groups = false;
                Linker->Type = LinkerType::GNU;
                Linker->file = p;
                SW_UNIMPLEMENTED;
                //Linker->Prefix = s.Settings.TargetOS.getLibraryPrefix();
                //Linker->Extension = s.Settings.TargetOS.getStaticLibraryExtension();

                auto lopts2 = LOpts;
                //lopts2.System.LinkLibraries.push_back("c++"); // remove and add to progs explicitly?
                //lopts2.System.LinkLibraries.push_back("c++fs"); // remove and add to progs explicitly?

                *Linker = lopts2;
                //s.registerProgram(appleclang ? "com.apple.LLVM.ld" : "org.LLVM.ld", Linker);

                if (s.getHostOs().is(OSType::Macos))
                {
                    if (!appleclang)
                        Linker->GNULinkerOptions::LinkDirectories().push_back(p.parent_path().parent_path() / "lib");
                }

                NativeCompilerOptions COpts;

                // C
                {
                    bool appleclang = is_apple_clang(p);

                    auto C = std::make_shared<ClangCompiler>(s.swctx);
                    C->Type = appleclang ? CompilerType::AppleClang : CompilerType::Clang;
                    C->file = p;
                    *C = COpts;
                    // also with asm
                    // .s - pure asm
                    // .S - with #define (accepts -D) and #include (accepts -I), also .sx
                    //C->input_extensions = { ".c", ".s", ".S" };
                    //s.registerProgram(appleclang ? "com.apple.LLVM.clang" : "org.LLVM.clang", C);

                    if (!macos_sdk_dir.empty())
                        C->IncludeSystemRoot = macos_sdk_dir;
                }
            }
        }*/

        SW_UNIMPLEMENTED;
        /*for (auto &v : clangpp_vers)
        {
            p = resolve(v);
            if (!p.empty())
            {
                // CPP
                {
                    bool appleclang = is_apple_clang(p);

                    auto C = std::make_shared<ClangCompiler>(s.swctx);
                    C->Type = appleclang ? CompilerType::AppleClang : CompilerType::Clang;
                    C->file = p;
                    *C = COpts;
                    //C->input_extensions = getCppSourceFileExtensions();
                    //s.registerProgram(appleclang ? "com.apple.LLVM.clangpp" : "org.LLVM.clangpp", C);

                    if (!macos_sdk_dir.empty())
                        C->IncludeSystemRoot = macos_sdk_dir;
                }
            }
        }*/
    }
}

void detectNativeCompilers(DETECT_ARGS)
{
    auto &os = s.getHostOs();
    if (os.is(OSType::Windows) || os.is(OSType::Cygwin))
    {
        if (os.is(OSType::Cygwin))
            detectNonWindowsCompilers(s);
        detectWindowsCompilers(s);
    }
    else
        detectNonWindowsCompilers(s);
}

}