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
    STEAM,
    ORIGIN,
    RETAIL
};

GameRelease GetReleaseVersion();
bool PatchInstruction(HANDLE hProcess, uintptr_t address, void* code);
void ApplyRA3Patches();