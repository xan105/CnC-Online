/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <string>

std::wstring Getenv(LPCWSTR name);
void enableConsole();