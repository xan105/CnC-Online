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
#include <atomic>
#include <iostream>
#include "../vendor/Detour/detours.h"

#if defined(_X86_)
  #pragma comment(lib, "../vendor/Detour/detours.x86.lib")
#elif defined(_AMD64_)
  #pragma comment(lib, "../vendor/Detour/detours.x64.lib")
#endif
#pragma comment(lib, "ws2_32.lib") //ntohs & htons

typedef HINSTANCE(WINAPI* ShellExecuteW_t)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);
typedef int (WSAAPI *connect_t)(SOCKET s, const sockaddr*, int namelen);
typedef int (WSAAPI *send_t)(SOCKET s, const char *buf, int len, int flags);
typedef struct hostent* (WSAAPI *gethostbyname_t)(const char *name);

HINSTANCE WINAPI detourShellExecuteW(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd);
int WSAAPI detourConnect(SOCKET s, const sockaddr* name, int namelen);
int WSAAPI detourSend(SOCKET s, const char *buf, int len, int flags);
struct hostent* WSAAPI detourGetHostByName(const char *name);
bool ModifyPublicKey();
bool takeDetour(PVOID* ppPointer, PVOID pDetour);
bool setDetoursForSocket();
bool setDetoursForShell();
DWORD WINAPI Main(LPVOID lpReserved);
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);
