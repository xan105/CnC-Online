/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.

Based on https://github.com/lanyizi/BegoneCrashers
All credits to lanyizi
*/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <unordered_map>
#include <psapi.h>
#include <string>
#include <vector>
#include <iostream>
#include "wall_crash.h"

enum GameRelease {
    UNKNOWN,
    RETAIL,   //SecuROM (DVD and legacy Steam version)
    DIGITAL,  //Online store (EA/Origin and Steam version)
};

using AddressMap = std::unordered_map<GameRelease, uintptr_t>;
using GameVersionMap = std::unordered_map<std::string, AddressMap>;

GameRelease GetReleaseVersion(std::string version);
bool PatchInstruction(HANDLE hProcess, uintptr_t address, void* code);
void ApplyRA3Patches();