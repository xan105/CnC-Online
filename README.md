# Open Source - Revora/CnC-Online Client

> C&C:Online is a community-made and -managed online server for Generals, Zero Hour, Tiberium Wars, Kane's Wrath, and Red Alert 3, allowing you to log in and continue playing online just like you could when GameSpy's servers were still online.

## Description

This project provides a patch through DLL injection or DLL sideloading to restore online play features in games such as *Red Alert 3* using the [Revora/CnC-Online](https://cnc-online.net/en/) server. It serves as an alternative to the official CnC-Online launcher which use EasyHook for the patching process. 

🐧 Running Linux ? This patch is compatible with Linux/Proton.

> [!NOTE]
> This patch was mainly tested with Red Alert 3, which is the primary focus of this project, but it works just fine with the other games supported by [Revora/CnC-Online](https://cnc-online.net/en/).

## Usage

### A) DLL Sideloading (easy)

This patch DLL can act as a proxy to `winmm.dll` located in `C:\Windows\System32\`.

Rename `opencnconline.dll` to `winmm.dll` and copy it next to the game's executable (**not** the game's launcher).

Example:

- Red Alert 3: 
    + Exec = `RA3.exe` ❌ -> `\Data\ra3_1.13.game` ✔️
    + DLL = `\Data\winmm.dll`
- C&C3 Kane's Wrath: 
    + Exec = `CNC3EP1.exe` ❌ -> `\RetailExe\1.3\cnc3ep1.dat` ✔️
    + DLL = `\RetailExe\1.3\winmm.dll`
- C&C3 Tiberium Wars: 
    + Exec = `CNC3.exe` ❌ -> `\RetailExe\1.10\cnc3game.dat` ✔️
    + DLL = `\RetailExe\1.10\winmm.dll`    
- Zero Hour: 
    + Exec = `Generals.exe` ❌ -> `\game.dat` ✔
    + DLL = `\winmm.dll`

🐧 Linux/Proton: Open up the game properties in Steam and add `WINEDLLOVERRIDES="winmm=n,b" %command%` to the launch options. Then launch the game as you would normally do.

### B) DLL Injection (advanced)

This patch was originally designed to work with my [RA3.exe re-implementation / alternative](https://github.com/xan105/RA3-Launcher), but it can also be used with any DLL injection tool of your choice.

A quick google search will find you plenty on GitHub.<br />
🐧 Linux: the classic combo `createRemoteThread()` + `LoadLibrary()` from `Kernel32` works under Wine/Proton.

You need to inject the DLL into the game process and **not** the launcher.

Example:

- Red Alert 3: 
    + Exec = `RA3.exe` ❌ -> `\Data\ra3_1.13.game` ✔️
    + Args = `-config "%GAMEDIR%\RA3_english_1.13.SkuDef"`
- C&C3 Kane's Wrath: 
    + Exec = `CNC3EP1.exe` ❌ -> `\RetailExe\1.3\cnc3ep1.dat` ✔️
    + Args = `-config "%GAMEDIR%\CNC3EP1_english_1.3.SkuDef"`
- C&C3 Tiberium Wars: 
    + Exec = `CNC3.exe` ❌ -> `\RetailExe\1.10\cnc3game.dat` ✔️
    + Args = `-config "%GAMEDIR%\CNC3_english_1.10.SkuDef"`
- Zero Hour: 
    + Exec = `Generals.exe` ❌ -> `\game.dat` ✔️

## How does it work ?

Upon injection into the game process, the patch DLL performs the following actions:

- Find and replace the original EA public key with the CnC-Online key (if any).
- Hook [send()](https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-send) and [gethostbyname()](https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-gethostbyname) calls to redirect them to the CnC Online GameSpy server emulation service.
- Hook [connect()](https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-connect) to switch to an alternative peerchat port in case port `6667` is blocked.<br /> Some countries do block this port because it is also used by IRC.

<p align="center">
  <img src="https://github.com/xan105/CnC-Online/raw/main/screenshot/welcome_back_commander.png">
  <em>Connected to C&C:Online</em>
</p>

## Why not use the official CnC-Online launcher ?

_"Launchers inception"_ (Launcher that starts another Launcher) is despised by many but it remains a matter of personal preference. 
For me the core issue was that their launcher **did not work** with the Steam version nor with Linux/Proton when I tried it.

And I'm absolutely not a fan of registering their launcher as a debugger in the registry for specific executables (what they call "hook" mode in their launcher).

I aimed to restore the online features of *Red Alert 3* without relying on their launcher and make the solution compatible with Linux/Proton.

<p align="center">
  <img src="https://github.com/xan105/CnC-Online/raw/main/screenshot/linux_proton.png">
  <em>Connected to C&C:Online under 🐧 Linux/Proton 9.0-2 (Fedora)</em>
</p>

## Env Var

### `RA3_PATCH=ON|OFF` (ON)

> "Wall Crash" Fix.

When enabled patches a "NULL pointer dereference" game crash caused by a player ordering some structures (for example, wall) to move.<br />
Apparently this is a common occurence when playing online where a griever will crash the game for the others when loosing.<br />
This patch is enabled by default.

NB: Red Alert 3 v1.12 only.

## Reminder for Online Play

### Registry
  
  Please be advice that these games check the values in the registry: incorrect or missing value(s) may prompt an "offline" error when entering "Online play" in the menu even tho your network and the server are fine (not to be confused with actual network error).

### NAT

  These games use IPv4 only and are P2P meaning you most likely need port forwarding to play online.
  If you are playing with your mates consider using a VPN (hamachi, radmin, ...) even for online play as a work-around.
  
  <details>
  <summary>Red Alert 3 ports:</summary>

  |PORT|DESCRIPTION|
  |----|-----------|
  |TCP/3783|RA3 Voice Chat Port|
  |TCP/4321|RA3 Mangler Servers|
  |TCP/28900|RA3 Master Server List Request|
  |TCP/29900|GP Connection Manager|
  |TCP/29901|GP Search Manager|
  |TCP/16000|Backend Server|
  |UDP/6500|RA3 Query Port|
  |UDP/6515|RA3 Dplay UDP|
  |UDP/13139|RA3 Custom UDP Pings|
  |UDP/27900|RA3 Master Server UDP Heartbeat|
  |UDP/16000|Backend Server|

  </details>

### Network interface

  Being P2P these games generally have an option in their settings menu to choose which IP/Network adapter to use.
  Usually the default is fine and correct but sometimes it isn't.

Build
=====

🆚 **Visual Studio 2022**

📦 Vendor dependencies:
  - [Microsoft Detours library](https://github.com/microsoft/Detours)

Solution: `./vc/opencnconline.sln`<br />
Output: `./build/output/${platform}/${config}`
