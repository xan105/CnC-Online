/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.

Based on https://github.com/lanyizi/BegoneCrashers
All credits to lanyizi
*/

#include "patcher.h"
#include "../../memory.h"
#include "wall_crash.h"

const GameVersionMap VERSIONS = {
  {"1.12", {
    { DIGITAL, 0xC6262C },
    { RETAIL, 0xC5B6C4 }
  }},
  {"1.13", {
    { DIGITAL, 0xC64DBC }
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

bool PatchInstruction(HANDLE hProcess, uintptr_t address, void* patch) {
    const SIZE_T jmpSize = 5;
    std::vector<BYTE> instruction(jmpSize, 0);
    instruction[0] = 0xE9; //'jmp' opcode
    std::int32_t offset = static_cast<std::int32_t>(
        reinterpret_cast<uintptr_t>(patch) - (address + jmpSize)
    );
    std::memcpy(instruction.data() + 1, &offset, sizeof(offset));
    
    if(!WriteBytesToMemory(hProcess, reinterpret_cast<LPVOID>(address), 0, instruction)){
        return false;
    }

    return FlushInstructionCache(hProcess, nullptr, 0);
}

bool GetFileVersion(TCHAR* szPath, int& major, int& minor) {
    DWORD verHandle = 0;
    DWORD verSize = GetFileVersionInfoSizeW(szPath, &verHandle);
    if (verSize == 0 || verHandle != 0) return false;

    LPBYTE verData = new BYTE[verSize];
    if (!GetFileVersionInfoW(szPath, 0, verSize, verData)){
        delete[] verData;
        return false;
    }

    VS_FIXEDFILEINFO* verInfo = NULL;
    UINT size = 0;

    if (!VerQueryValueW(verData, L"\\", (LPVOID*)&verInfo, &size) || size == 0){
        delete[] verData;
        return false;
    }

    major = HIWORD(verInfo->dwFileVersionMS);
    minor = LOWORD(verInfo->dwFileVersionMS);

    delete[] verData;
    return true;
}

const std::wstring GetFileName(TCHAR* szPath) {
    std::wstring execPath = szPath;
    size_t lastSlash = execPath.find_last_of(L"\\");
    return lastSlash != std::wstring::npos ? execPath.substr(lastSlash + 1) : execPath;
}

void ApplyRA3Patches() {
    HANDLE hProcess = GetCurrentProcess();
    TCHAR buffer[MAX_PATH] = { 0 };

    if (GetModuleFileNameExW(hProcess, NULL, buffer, sizeof(buffer) / sizeof(TCHAR))) {
        const std::wstring execName = GetFileName(buffer);
        int major, minor = 0;
        if (GetFileVersion(buffer, major, minor)) {
            if (execName == L"ra3_1.12.game" && major == 1 && minor == 12) 
            {
                std::cout << "Red Alert 3 (v1.12): ";
                switch (GetReleaseVersion("1.12")) {
                    case DIGITAL: {
                        std::cout << "Digital Release (EA/Origin, Steam)." << std::endl;
                        if (PatchInstruction(hProcess, 0x590048, &WallCrash_patch_0) &&
                            PatchInstruction(hProcess, 0x85B386, &WallCrash_patch_1)) {
                            std::cout << "Applied RA3 \"Wall Crash\" fix." << std::endl;
                        }
                        break;
                    }
                    case RETAIL: {
                        std::cout << "Retail Release (SecuROM DVD / Steam)." << std::endl;
                        if (PatchInstruction(hProcess, 0x54EA88, &WallCrash_patch_0) &&
                            PatchInstruction(hProcess, 0x81D1F6, &WallCrash_patch_1)) {
                            std::cout << "Applied RA3 \"Wall Crash\" fix." << std::endl;
                        }
                        break;
                    }
                    default: {
                        std::cout << "Unknown Release." << std::endl;
                        break;
                    }
                }
            }
            else if (execName == L"ra3_1.13.game" && major == 1 && minor == 13) 
            {
                std::cout << "Red Alert 3 (v1.13): ";
                switch (GetReleaseVersion("1.13")) {
                    case DIGITAL: {
                        std::cout << "Digital Release (EA/Origin, Steam)." << std::endl;
                        if (PatchInstruction(hProcess, 0x5790C8, &WallCrash_patch_0) &&
                            PatchInstruction(hProcess, 0x8466A6, &WallCrash_patch_1)) {
                            std::cout << "Applied RA3 \"Wall Crash\" fix." << std::endl;
                        }
                        break;
                    }
                    default: {
                        std::cout << "Unknown Release." << std::endl;
                        break;
                    }
                }
            }
        }
    }
    CloseHandle(hProcess);
}