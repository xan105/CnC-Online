/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <memory>
#include <string>
#include <algorithm>
#include <vector>

bool ReadMemory(HANDLE processHandle, LPCVOID baseAddress, SIZE_T size, std::vector<BYTE>& buffer);
bool FindBytesInMemory(const std::vector<BYTE>& memory, const std::vector<BYTE>& pattern, SIZE_T& offset);
bool WriteBytesToMemory(HANDLE processHandle, LPVOID baseAddress, SIZE_T offset, const std::vector<BYTE>& newPattern);
