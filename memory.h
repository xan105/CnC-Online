/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include <Windows.h>
#include <iostream>
#include <memory>
#include <string>
#include <algorithm>
#include <vector>

bool ReadMemory(HANDLE processHandle, LPCVOID baseAddress, SIZE_T size, std::vector<BYTE>& buffer);
bool FindBytesInMemory(const std::vector<BYTE>& memory, const std::vector<BYTE>& pattern, SIZE_T& foundOffset);
bool WriteBytesToMemory(HANDLE processHandle, LPVOID baseAddress, const std::vector<BYTE>& newPattern, SIZE_T offset);
