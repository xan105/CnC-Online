/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "memory.h"

bool ReadMemory(HANDLE processHandle, LPCVOID baseAddress, SIZE_T size, std::vector<BYTE>& buffer) {
    buffer.resize(size);
    SIZE_T bytesRead;
    return ReadProcessMemory(processHandle, baseAddress, buffer.data(), size, &bytesRead) && bytesRead == size;
}

bool FindBytesInMemory(const std::vector<BYTE>& memory, const std::vector<BYTE>& pattern, SIZE_T& offset) {
    auto it = std::search(memory.begin(), memory.end(), pattern.begin(), pattern.end());

    if (it != memory.end()) {
        offset = std::distance(memory.begin(), it);
        return true;
    }
    return false;
}

bool WriteBytesToMemory(HANDLE processHandle, LPVOID baseAddress, SIZE_T offset, const std::vector<BYTE>& newPattern) {
    // Calculate the target address
    BYTE* targetAddress = (BYTE*)baseAddress + offset;

    // Change the protection of the memory region
    DWORD oldProtect;
    SIZE_T size = newPattern.size();
    if (!VirtualProtectEx(processHandle, targetAddress, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        std::cerr << "Failed to change memory protection." << std::endl;
        return false;
    }

    // Write to the memory
    bool writeSuccess = WriteProcessMemory(processHandle, targetAddress, newPattern.data(), newPattern.size(), NULL);

    // Restore the old protection
    if (!VirtualProtectEx(processHandle, targetAddress, size, oldProtect, &oldProtect)) {
        std::cerr << "Failed to restore memory protection." << std::endl;
        return false;
    }

    return writeSuccess;
}
