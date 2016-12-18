/*
 * Copyright (c) 2016, Egor Pugin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the copyright holder nor the names of
 *        its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <bitset>
#include <string>

enum class ProjectType
{
    None,
    Library,
    Executable,
    RootProject,
    Directory,
};

enum class LibraryType
{
    Static,
    Shared,
    Module, // from CMake
};

enum class ExecutableType
{
    Default,
    Win32,
};

enum class ProjectPathNamespace
{
    None,       // invalid

    com = 1,    // closed-or-commercial-source
    org,        // open-source
    pvt,        // users' packages
    // pub?
    // std?
};

// append only!
enum ProjectFlag
{
    // version flag = vf
    // project flag = pf
    // dependency flag = df

    pfHeaderOnly                = 0,    // vf
    //pfUnstable                  = 1,    // vf, unused, not pfFixed
    //pfNonsecure                 = 2,    // vf, unused, pf, show warning, security flaws
    //pfOutdated                  = 3,    // vf, unused, project is considered for removal
    //pfNonOfficial               = 4,    // pf, unused,
    //pfFixed                     = 5,    // vf, unused, version is fixed and cannot be removed
    pfExecutable                = 6,    // pf
    //pfEmpty                     = 7,    // vf, unused, can be used to load & include cmake packages
    pfPrivateDependency         = 8,    // df, private dependency
    pfDirectDependency          = 9,    // vf, response only
    pfIncludeDirectoriesOnly    = 10,   // df, take include dirs from this dep
    pfLocalProject              = 11,   // vf, not from server, local bs project

    //pfPreferBinary,   //pf, if binaries are available, do not build project, use precompiled

    //pfLibrary?,                  // pf
    //pfRootProject?,              // pf
    //pfDirectory?,                // pf

    // pfOptional?
};

enum class NotificationType
{
    None,
    Message,
    Success,
    Warning,
    Error,
};

enum class SettingsType
{
    None,   // use default (user)
    Local,  // in current project, dir: cppan
    User,   // in user package store
    System, // in system package store
    Max,
};

using ProjectFlags = std::bitset<sizeof(uint64_t) * 8>;

template <typename E>
constexpr std::underlying_type_t<E> toIndex(E e)
{
    return static_cast<std::underlying_type_t<E>>(e);
}

std::string toString(ProjectType e);
std::string toString(ProjectPathNamespace e);
std::string toString(SettingsType e);

std::string getFlagsString(const ProjectFlags &flags);
