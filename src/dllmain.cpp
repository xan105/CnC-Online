/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "dllmain.h"
#include "memory.h"
#include "util.h"
#include "patch/RA3/patcher.h"

connect_t pConnect = nullptr;
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

HINSTANCE WINAPI detourShellExecuteW(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd) {
    std::cout << "ShellExecuteW()" << std::endl;

    if (lpOperation && wcscmp(lpOperation, L"open") == 0) {
        std::wstring file(lpFile);
        if (file == L"IEXPLORE.EXE") {                                                                  //Kane's Wrath
            std::wstring param(lpParameters);
            if (param == L"http://www.ea.com/global/legal/tos.jsp") {
                file = L"https://cnc-online.net/en/faq/";
            }
            else if (param == L"http://www.commandandconquer.com") {
                file = L"https://cnc-online.net/en/";
            }
        }
        else if (file == L"http://profile.ea.com/" ||                                                   //RA3
                (file.size() >= 8 && _wcsicmp(file.c_str() + file.size() - 8, L"EREG.EXE") == 0)) {     //Kane's Wrath
            file = L"https://cnc-online.net/en/connect/register/";
        }
        else if (file.find(L"http://www.ea.com/redalert/") == 0 ) {                                     //RA3
            file = L"https://cnc-online.net/en/";
        }

        return pShellExecuteW(hwnd, lpOperation, file.c_str(), NULL, lpDirectory, nShowCmd);
    }

    return pShellExecuteW(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
}

std::atomic<bool> useAltPeerChatPort(false);
int WSAAPI detourConnect(SOCKET s, const sockaddr* name, int namelen) { 
  std::cout << "Connect()" << std::endl;
  
  sockaddr_in* addr_in = (sockaddr_in*)name;
    if (addr_in->sin_family == AF_INET) { //IPv4
        int port = ntohs(addr_in->sin_port);
        if (port == 6667) {
            if (useAltPeerChatPort) {
                std::cout << "Using alt peer chat port" << std::endl;
                addr_in->sin_port = htons(16667);
            } else {
                int result = pConnect(s, name, namelen);
                if (result == SOCKET_ERROR) {
                    std::cout << "Switching to alt peer chat port" << std::endl;
                    useAltPeerChatPort = true;
                    addr_in->sin_port = htons(16667);
                    result = pConnect(s, name, namelen);
                }
                return result;
            }
        }
    }

    return pConnect(s, name, namelen);
}

int WSAAPI detourSend(SOCKET s, const char *buf, int len, int flags) {
    std::cout << "Send()" << std::endl;

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
    std::cout << "GetHostByName()" << std::endl;

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
                return true;
            }
            else {
                std::cerr << "Failed to write new pattern to memory." << std::endl;
                return false;
            }
        }
        else {
            std::cout << "Public key not found." << std::endl;
            return false;
        }
    }
    else {
        std::cerr << "Failed to read process memory." << std::endl;
        return false;
    }
}

bool takeDetour(PVOID* ppPointer, PVOID pDetour) {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(ppPointer, pDetour);
    return DetourTransactionCommit() == NO_ERROR;
}

bool setDetoursForSocket() {
    HMODULE hMod = LoadLibraryA("ws2_32.dll");
    if (hMod == nullptr) return false;

    pSend = (send_t)GetProcAddress(hMod, "send");
    if (pSend == nullptr) return false;
    if (!takeDetour(&(PVOID&)pSend, detourSend)) return false;

    pConnect = (connect_t)GetProcAddress(hMod, "connect");
    if (pConnect == nullptr) return false;
    if (!takeDetour(&(PVOID&)pConnect, detourConnect)) return false;
    
    pGetHostByName = (gethostbyname_t)GetProcAddress(hMod, "gethostbyname");
    if (pGetHostByName == nullptr) return false;
    if (!takeDetour(&(PVOID&)pGetHostByName, detourGetHostByName)) return false;

    return true;
}

bool setDetoursForShell() {
    HMODULE hMod = LoadLibraryA("shell32.dll");
    if (hMod == nullptr) return false;

    pShellExecuteW = (ShellExecuteW_t)GetProcAddress(hMod, "ShellExecuteW");
    if (pShellExecuteW == nullptr) return false;
    if (!takeDetour(&(PVOID&)pShellExecuteW, detourShellExecuteW)) return false;

    return true;
}

DWORD WINAPI Main(LPVOID lpReserved) {

    #ifdef _DEBUG
        enableConsole();
    #endif

    std::cout << "pid: " << GetCurrentProcessId() << std::endl;

    if (setDetoursForSocket() && 
        setDetoursForShell()) { 
        std::cout << "Detour function set." << std::endl;
    } else {
        std::cerr << "Failed to set detour function." << std::endl;
    }

    if (ModifyPublicKey()) {
        std::cout << "Public key modified successfully." << std::endl;
    }
    else {
        std::cerr << "Failed to modify public key." << std::endl;
    }
    
    ApplyRA3Patches();

    return 0;
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