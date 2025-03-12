/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "dllmain.h"
#include "memory.h"
#include "util.h"
#include "online.h"
#include "patch/RA3/patcher.h"

connect_t pConnect = nullptr;
send_t pSend = nullptr;
gethostbyname_t pGetHostByName = nullptr;
ShellExecuteW_t pShellExecuteW = nullptr;

HINSTANCE WINAPI detourShellExecuteW(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd) {
    std::cout << "ShellExecuteW()" << std::endl;

    if (lpOperation && wcscmp(lpOperation, L"open") == 0) {
        std::wstring file(lpFile);
        if (file == L"IEXPLORE.EXE") {                                                                  //Kane's Wrath
            std::wstring param(lpParameters);
            if (param == L"http://www.ea.com/global/legal/tos.jsp") {
                file = toWString(hostname.at("tos"));   
            }
            else if (param == L"http://www.commandandconquer.com") {
                file = toWString(hostname.at("website"));
            }
        }
        else if (file == L"http://profile.ea.com/" ||                                                   //RA3
                (file.size() >= 8 && _wcsicmp(file.c_str() + file.size() - 8, L"EREG.EXE") == 0)) {     //Kane's Wrath
            file = toWString(hostname.at("register"));
        }
        else if (file.find(L"http://www.ea.com/redalert/") == 0 ) {                                     //RA3
            file = toWString(hostname.at("website"));
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
        if (port == PORT_PEERCHAT) {
            if (useAltPeerChatPort) {
                std::cout << "Using alt peer chat port" << std::endl;
                addr_in->sin_port = htons(PORT_PEERCHAT_ALT);
            } else {
                int result = pConnect(s, name, namelen);
                if (result == SOCKET_ERROR) {
                    std::cout << "Switching to alt peer chat port" << std::endl;
                    useAltPeerChatPort = true;
                    addr_in->sin_port = htons(PORT_PEERCHAT_ALT);
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
        std::string newHost = "Host: " + hostname.at("host"); 

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
        host = hostname.at("host");
    }
    else if (host == "bfme.fesl.ea.com" || 
             host == "bfme2.fesl.ea.com" || 
             host == "bfme2-ep1-pc.fesl.ea.com" ||
             host == "cnc3-pc.fesl.ea.com" || 
             host == "cnc3-ep1-pc.fesl.ea.com" || 
             host == "cncra3-pc.fesl.ea.com") 
    {
        host = hostname.at("login");
    }
    else if (host == "gpcm.gamespy.com") 
    {
        host = hostname.at("gpcm");
    }
    else if (host == "peerchat.gamespy.com") 
    {
        host = hostname.at("peerchat");
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
        host = hostname.at("master");
    }
    else if (host == "redalert3pc.natneg1.gamespy.com" || 
             host == "redalert3pc.natneg2.gamespy.com" ||
             host == "redalert3pc.natneg3.gamespy.com") 
    {
        host = hostname.at("natneg");
    }
    else if (host == "lotrbme.gamestats.gamespy.com" || 
             host == "lotrbme2r.gamestats.gamespy.com" ||
             host == "gamestats.gamespy.com") 
    {
        host = hostname.at("stats");
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
        host = hostname.at("sake");
    }
    else if (host == "lotrbfme.arenasdk.gamespy.com" || 
             host == "arenasdk.gamespy.com" ||
             host == "launch.gamespyarcade.com" || 
             host == "www.gamespy.com" ||
             host == "ingamead.gamespy.com") 
    {
        host = hostname.at("server");
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
        SIZE_T offset;
        if (FindBytesInMemory(memory, eaPublicKey, offset)) {
            std::cout << "Public key found at offset: 0x" << std::hex << offset << " (" << std::dec << offset << ")" << std::endl;
            if (WriteBytesToMemory(processHandle, moduleInfo.lpBaseOfDll, offset, cncOnlinePublicKey)) {
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
    
    bool patch = Getenv(L"RA3_PATCH") == L"ON" ? true : false;
    if(patch) {
        ApplyRA3Patches();
    } else {
        std::cout << "Skipping RA3 patches..." << std::endl;
    }

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