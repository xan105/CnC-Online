/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "pch.h"
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <Psapi.h>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include "vendor/Detour/detours.h"

#if defined(_X86_)
#pragma comment(lib, "vendor/Detour/detours.x86.lib")
#elif defined(_AMD64_)
#pragma comment(lib, "vendor/Detour/detours.x64.lib")
#endif

typedef int (WSAAPI *send_t)(SOCKET s, const char *buf, int len, int flags);
typedef struct hostent* (WSAAPI *gethostbyname_t)(const char *name);
typedef HINSTANCE(WINAPI* ShellExecuteW_t)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);

send_t pSend = nullptr;
gethostbyname_t pGetHostByName = nullptr;
ShellExecuteW_t pShellExecuteW = nullptr;

std::vector<BYTE> eaPublicKey = {
    0x92, 0x75, 0xA1, 0x5B, 0x08, 0x02, 0x40, 0xB8,
    0x9B, 0x40, 0x2F, 0xD5, 0x9C, 0x71, 0xC4, 0x51,
    0x58, 0x71, 0xD8, 0xF0, 0x2D, 0x93, 0x7F, 0xD3,
    0x0C, 0x8B, 0x1C, 0x7D, 0xF9, 0x2A, 0x04, 0x86,
    0xF1, 0x90, 0xD1, 0x31, 0x0A, 0xCB, 0xD8, 0xD4,
    0x14, 0x12, 0x90, 0x3B, 0x35, 0x6A, 0x06, 0x51,
    0x49, 0x4C, 0xC5, 0x75, 0xEE, 0x0A, 0x46, 0x29,
    0x80, 0xF0, 0xD5, 0x3A, 0x51, 0xBA, 0x5D, 0x6A,
    0x19, 0x37, 0x33, 0x43, 0x68, 0x25, 0x2D, 0xFE,
    0xDF, 0x95, 0x26, 0x36, 0x7C, 0x43, 0x64, 0xF1,
    0x56, 0x17, 0x0E, 0xF1, 0x67, 0xD5, 0x69, 0x54,
    0x20, 0xFB, 0x3A, 0x55, 0x93, 0x5D, 0xD4, 0x97,
    0xBC, 0x3A, 0xD5, 0x8F, 0xD2, 0x44, 0xC5, 0x9A,
    0xFF, 0xCD, 0x0C, 0x31, 0xDB, 0x9D, 0x94, 0x7C,
    0xA6, 0x66, 0x66, 0xFB, 0x4B, 0xA7, 0x5E, 0xF8,
    0x64, 0x4E, 0x28, 0xB1, 0xA6, 0xB8, 0x73, 0x95
};

std::vector<BYTE> cncOnlinePublicKey = {
    0xE7, 0xCD, 0xB7, 0xD8, 0x15, 0x51, 0xC5, 0xD7,
    0xED, 0xBE, 0x0D, 0x59, 0x6A, 0xCC, 0x45, 0x72,
    0x03, 0xFB, 0xE0, 0x08, 0x5A, 0x7C, 0x75, 0xBC,
    0xBF, 0x50, 0x7A, 0x24, 0xE2, 0x5A, 0x4A, 0x71,
    0x31, 0x63, 0x07, 0xD8, 0xDE, 0x03, 0x96, 0xF6,
    0x84, 0x18, 0x1A, 0xAB, 0xC9, 0x55, 0x4B, 0x4F,
    0x87, 0x9F, 0x43, 0x63, 0x43, 0x72, 0xBF, 0x6F,
    0xF0, 0x72, 0x66, 0x95, 0x3A, 0x63, 0xE5, 0x41,
    0xAE, 0x6F, 0x58, 0xD9, 0xCF, 0x23, 0xFA, 0x49,
    0x0B, 0x3C, 0xEF, 0x28, 0xE5, 0x51, 0xF6, 0x99,
    0x37, 0x51, 0xE7, 0xEF, 0x43, 0x12, 0xB4, 0x1B,
    0x59, 0x31, 0x5B, 0x55, 0x05, 0x51, 0x0C, 0x3C,
    0xC0, 0x51, 0x28, 0x43, 0x28, 0xFB, 0x00, 0x51,
    0x74, 0x7C, 0x2C, 0xDA, 0x5A, 0x84, 0xDE, 0xC1,
    0xD0, 0x40, 0x26, 0x32, 0xC1, 0xA1, 0x80, 0xAD,
    0x0D, 0x07, 0xE5, 0xAD, 0x93, 0x0D, 0x2D, 0xF5
};

HINSTANCE WINAPI detourShellExecuteW(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd)
{
    std::wstring file(lpFile);

    if (wcscmp(lpOperation, L"open") == 0) {
        if (file == L"http://profile.ea.com/") { //RA3 Register Account button
            file = L"https://cnc-online.net/en/connect/register/";
        }
        else if (file.find(L"http://www.ea.com/redalert/") == 0 ) { //RA3 CnC Website button
            file = L"https://cnc-online.net/en";
        }
    }

    return pShellExecuteW(hwnd, lpOperation, file.c_str(), lpParameters, lpDirectory, nShowCmd);
}


int WSAAPI detourSend(SOCKET s, const char *buf, int len, int flags) {
    std::string str(buf, len);

    if (str.find("GET ") == 0 || str.find("HEAD ") == 0) {

        std::string updatedStr = str;
        std::string targetHost = "Host: na.llnet.eadownloads.ea.com";
        std::string newHost = "Host: http.server.cnc-online.net"; 

        size_t pos = updatedStr.find(targetHost);
        if (pos != std::string::npos) {
            updatedStr.replace(pos, targetHost.length(), newHost);
        }

        char* updatedBuf = new char[updatedStr.length() + 1];
        strcpy_s(updatedBuf, updatedStr.length() + 1, updatedStr.c_str());

        int result = pSend(s, updatedBuf, static_cast<int>(updatedStr.length()), flags);

        delete[] updatedBuf;

        return result;
    }

    return pSend(s, buf, len, flags);
}

struct hostent* WSAAPI detourGetHostByName(const char *name) {
    std::string host(name);
    
    if (host == "servserv.generals.ea.com" || 
        host == "na.llnet.eadownloads.ea.com") 
    {
        host = "http.server.cnc-online.net";
    }
    else if (host == "bfme.fesl.ea.com" || 
             host == "bfme2.fesl.ea.com" || 
             host == "bfme2-ep1-pc.fesl.ea.com" ||
             host == "cnc3-pc.fesl.ea.com" || 
             host == "cnc3-ep1-pc.fesl.ea.com" || 
             host == "cncra3-pc.fesl.ea.com") 
    {
        host = "login.server.cnc-online.net";
    }
    else if (host == "gpcm.gamespy.com") 
    {
        host = "gpcm.server.cnc-online.net";
    }
    else if (host == "peerchat.gamespy.com") 
    {
        host = "peerchat.server.cnc-online.net";
    }
    else if (host == "lotrbme.available.gamespy.com" || 
             host == "lotrbme.master.gamespy.com" ||
             host == "lotrbme.ms13.gamespy.com" || 
             host == "lotrbme2r.available.gamespy.com" ||
             host == "lotrbme2r.master.gamespy.com" || 
             host == "lotrbme2r.ms9.gamespy.com" ||
             host == "ccgenerals.ms19.gamespy.com" || 
             host == "ccgenzh.ms6.gamespy.com" ||
             host == "cc3tibwars.available.gamespy.com" || 
             host == "cc3tibwars.master.gamespy.com" ||
             host == "cc3tibwars.ms17.gamespy.com" || 
             host == "cc3xp1.available.gamespy.com" ||
             host == "cc3xp1.master.gamespy.com" || 
             host == "cc3xp1.ms18.gamespy.com" ||
             host == "redalert3pc.available.gamespy.com" || 
             host == "redalert3pc.master.gamespy.com" ||
             host == "redalert3pc.ms1.gamespy.com" || 
             host == "master.gamespy.com") 
    {
        host = "master.server.cnc-online.net";
    }
    else if (host == "redalert3pc.natneg1.gamespy.com" || 
             host == "redalert3pc.natneg2.gamespy.com" ||
             host == "redalert3pc.natneg3.gamespy.com") 
    {
        host = "natneg.server.cnc-online.net";
    }
    else if (host == "lotrbme.gamestats.gamespy.com" || 
             host == "lotrbme2r.gamestats.gamespy.com" ||
             host == "gamestats.gamespy.com") 
    {
        host = "gamestats.server.cnc-online.net";
    }
    else if (host == "cc3tibwars.auth.pubsvs.gamespy.com" || 
             host == "cc3tibwars.comp.pubsvs.gamespy.com" ||
             host == "cc3tibwars.sake.gamespy.com" || 
             host == "cc3xp1.auth.pubsvs.gamespy.com" ||
             host == "cc3xp1.comp.pubsvs.gamespy.com" || 
             host == "cc3xp1.sake.gamespy.com" ||
             host == "redalert3pc.auth.pubsvs.gamespy.com" || 
             host == "redalert3pc.sake.gamespy.com" || 
             host == "redalert3services.gamespy.com" ||
             host == "psweb.gamespy.com") 
    {
        host = "sake.server.cnc-online.net";
    }
    else if (host == "lotrbfme.arenasdk.gamespy.com" || 
             host == "arenasdk.gamespy.com" ||
             host == "launch.gamespyarcade.com" || 
             host == "www.gamespy.com" ||
             host == "ingamead.gamespy.com") 
    {
        host = "server.cnc-online.net";
    }

    return pGetHostByName(host.c_str());
}

bool ReadMemory(HANDLE processHandle, LPCVOID baseAddress, SIZE_T size, std::vector<BYTE>& buffer) {
    buffer.resize(size);
    SIZE_T bytesRead;
    return ReadProcessMemory(processHandle, baseAddress, buffer.data(), size, &bytesRead) && bytesRead == size;
}

bool FindBytesInMemory(const std::vector<BYTE>& memory, const std::vector<BYTE>& pattern, SIZE_T& foundOffset) {
    auto it = std::search(memory.begin(), memory.end(), pattern.begin(), pattern.end());

    if (it != memory.end()) {
        foundOffset = std::distance(memory.begin(), it);
        return true;
    }
    return false;
}

bool WriteBytesToMemory(HANDLE processHandle, LPVOID baseAddress, const std::vector<BYTE>& newPattern, SIZE_T offset) {
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

bool ModifyPublicKey() {

    MODULEINFO moduleInfo;
    HANDLE processHandle = GetCurrentProcess();

    if (!GetModuleInformation(processHandle, GetModuleHandle(NULL), &moduleInfo, sizeof(moduleInfo))) {
        std::cerr << "Failed to get module information." << std::endl;
        return false;
    }

    std::vector<BYTE> memory;
    if (ReadMemory(processHandle, moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage, memory)) {
        SIZE_T foundOffset;
        if (FindBytesInMemory(memory, eaPublicKey, foundOffset)) {
            std::cout << "Public key found at offset: " << foundOffset << std::endl;

            if (WriteBytesToMemory(processHandle, moduleInfo.lpBaseOfDll, cncOnlinePublicKey, foundOffset)) {
                std::cout << "Successfully wrote new pattern to memory." << std::endl;
                return TRUE;
            }
            else {
                std::cerr << "Failed to write new pattern to memory." << std::endl;
                return FALSE;
            }
        }
        else {
            std::cout << "Public key not found." << std::endl;
            return FALSE;
        }
    }
    else {
        std::cerr << "Failed to read process memory." << std::endl;
        return FALSE;
    }
}


void enableConsole() {

    if (AllocConsole()) {
        HWND consoleWindow = GetConsoleWindow();

        if (consoleWindow) {
            // Set the console window to be layered
            LONG style = GetWindowLong(consoleWindow, GWL_EXSTYLE);
            SetWindowLong(consoleWindow, GWL_EXSTYLE, style | WS_EX_LAYERED);
            SetLayeredWindowAttributes(consoleWindow, 0, 225, LWA_COLORKEY);
            
            // Show the console window
            ShowWindow(consoleWindow, SW_SHOW);
        }

        // Redirect stdio to the console
        FILE* dummy;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        freopen_s(&dummy, "CONIN$", "r", stdin);
        freopen_s(&dummy, "CONOUT$", "w", stderr);
    }
}

bool setDetours() {
 
    // Load the ws2_32.dll module
    static HMODULE ws2_32 = nullptr;
    ws2_32 = LoadLibrary(L"ws2_32.dll");
    if (ws2_32 == nullptr) {
        std::cerr << "Failed to load ws2_32.dll!" << std::endl;
        return FALSE;
    }
    // Get addresses of the functions to be hooked
    pSend = (send_t)GetProcAddress(ws2_32, "send");
    pGetHostByName = (gethostbyname_t)GetProcAddress(ws2_32, "gethostbyname");

    // Load the shell32.dll module
    static HMODULE shell32 = nullptr;
    shell32 = LoadLibrary(L"shell32.dll");
    if (shell32 == nullptr) {
        std::cerr << "Failed to load Shell32.dll!" << std::endl;
        return FALSE;
    }
    // Get addresses of the functions to be hooked
    pShellExecuteW = (ShellExecuteW_t)GetProcAddress(shell32, "ShellExecuteW");

    if (pSend == nullptr || pGetHostByName == nullptr || pShellExecuteW == nullptr) {
        std::cerr << "Failed to get function addresses!" << std::endl;
        return FALSE;
    }

    // Attach hooks
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pSend, detourSend);
    DetourAttach(&(PVOID&)pGetHostByName, detourGetHostByName);
    DetourAttach(&(PVOID&)pShellExecuteW, detourShellExecuteW);

    if (DetourTransactionCommit() != NO_ERROR) {
        std::cerr << "Failed to attach hooks!" << std::endl;
        return FALSE;
    }

    return true;
}

DWORD WINAPI Main(LPVOID lpReserved) {

    #ifdef _DEBUG
        enableConsole();
    #endif

    std::cout << "pid: " << GetCurrentProcessId() << std::endl;

    if (setDetours()) {
        std::cout << "Detour function set." << std::endl;
    }
    else {
        std::cerr << "Failed set detour function." << std::endl;
    }

    if (ModifyPublicKey()) {
        std::cout << "Public key modified successfully." << std::endl;
    }
    else {
        std::cerr << "Failed to modify public key." << std::endl;
    }

    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule);
            HANDLE hThread = CreateThread(nullptr, 0, Main, hModule, 0, nullptr);
            if (hThread) {
                CloseHandle(hThread);
            }
            break;
        }
    }
    return TRUE;
}
