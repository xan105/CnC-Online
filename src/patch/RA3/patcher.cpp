/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.

Based on https://github.com/lanyizi/BegoneCrashers
All credits to lanyizi
*/

#include "patcher.h"
#include "../../memory.h"

const GameVersionMap VERSIONS = {
  {"1.12", {
    {RETAIL, 0xC5B6C4},
    {DIGITAL, 0xC6262C}
  }},
  {"1.13", {
    {DIGITAL, 0x000000}, //TODO: 1.13 Memory address have changed
  }}
};

GameRelease GetReleaseVersion(std::string version) {
    for (const auto& [release, address] : VERSIONS.at(version)) {
        const char* ptr = reinterpret_cast<const char*>(address);
        if (ptr && std::string(ptr, 8) == "RedAlert") {
          return release;
        }
    }
    return static_cast<GameRelease>(UNKNOWN);
}

bool PatchInstruction(HANDLE hProcess, uintptr_t address, void* code) {
    
    const SIZE_T jmpSize = 5;
    std::vector<BYTE> instruction(jmpSize, 0);
    instruction[0] = 0xE9; //'jmp' opcode
    DWORD jmpTarget = reinterpret_cast<DWORD>(code);
    DWORD offset = jmpTarget - (address + jmpSize);
    std::memcpy(instruction.data() + 1, &offset, sizeof(offset));
    
    bool writeSuccess = WriteBytesToMemory(hProcess, reinterpret_cast<LPVOID>(address), instruction, jmpSize);
    if (!writeSuccess) {
        return false;
    }

    BOOL cacheFlushed = FlushInstructionCache(hProcess, reinterpret_cast<LPCVOID>(address), jmpSize);
    if (!cacheFlushed) {
        return false;
    }

    return true;
}

void ApplyRA3Patches() {

    HANDLE hProcess = GetCurrentProcess();
    TCHAR exePath[MAX_PATH];
    
    if (GetModuleFileNameExW(hProcess, NULL, exePath, sizeof(exePath) / sizeof(TCHAR))) {
        std::wstring executablePath = exePath;
        size_t lastSlash = executablePath.find_last_of(L"\\");
        std::wstring executableName = (lastSlash != std::wstring::npos) ? executablePath.substr(lastSlash + 1) : executablePath;
        
        if (executableName == L"ra3_1.12.game") { 
            std::cout << "Red Alert 3 (v1.12): ";
            switch (GetReleaseVersion("1.12")) {
                case RETAIL: {
                    std::cout << "Retail Release (SecuROM DVD / Steam)" << std::endl;
                    if (PatchInstruction(hProcess, 0x54EA88, &WallCrash_patch_retail) &&
                        PatchInstruction(hProcess, 0x81D1F6, &WallCrash_patch_common)) {
                      std::cout << "Applied RA3 \"Wall Crash\" fix" << std::endl;
                    }
                    break;
                }
                case DIGITAL: {
                    std::cout << "Digital Release (EA/Origin, Steam)" << std::endl;
                    if (PatchInstruction(hProcess, 0x590048, &WallCrash_patch_digital) &&
                        PatchInstruction(hProcess, 0x85B386, &WallCrash_patch_common)) {
                      std::cout << "Applied RA3 \"Wall Crash\" fix" << std::endl;
                    }
                    break;
                }
                default: {
                    std::cout << "Unknown Release" << std::endl;
                  break;
                }
            }
        }
    }
    
    CloseHandle(hProcess);
}