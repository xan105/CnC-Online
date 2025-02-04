/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Psapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "../vendor/Detour/detours.h"

#if defined(_X86_)
  #pragma comment(lib, "../vendor/Detour/detours.x86.lib")
#elif defined(_AMD64_)
  #pragma comment(lib, "../vendor/Detour/detours.x64.lib")
#endif

typedef int (WSAAPI *send_t)(SOCKET s, const char *buf, int len, int flags);
typedef struct hostent* (WSAAPI *gethostbyname_t)(const char *name);
typedef HINSTANCE(WINAPI* ShellExecuteW_t)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);

HINSTANCE WINAPI detourShellExecuteW(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd);
int WSAAPI detourSend(SOCKET s, const char *buf, int len, int flags);
struct hostent* WSAAPI detourGetHostByName(const char *name);
bool ModifyPublicKey();
void enableConsole();
bool setDetours();
DWORD WINAPI Main(LPVOID lpReserved);
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);
