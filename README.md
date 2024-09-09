# Open Source - Revora/CnC-Online Client

> C&C:Online is a community-made and -managed online server for Generals, Zero Hour, Tiberium Wars, Kane's Wrath, and Red Alert 3, allowing you to log in and continue playing online just like you could when GameSpy's servers were still online.

## Description

This project provides a patch through DLL injection to restore online play features in games such as *Red Alert 3* using the [Revora/CnC-Online](https://cnc-online.net/en/) server. It serves as an alternative to the official CnC-Online launcher which use EasyHook for the patching process.

This patch is designed to work with my [RA3.exe reimplementation/alternative](...), but it can also be used with any DLL injection tool of your choice.

**Note:** Although this patch should technically be compatible with the other supported games, testing has been limited to *Red Alert 3*, which is the primary focus of this project.

## How does it work ?

Upon injection into the game process, the patch DLL performs the following actions:

- Find and replace the original EA public key with the CnC-Online key.
- Hook _ws2_32_ (winsock2) `send()` and `gethostbyname()` Win32 API calls to redirect them to the CnC Online GameSpy server emulation service.

## Why not use the official CnC-Online launcher ?

While "Launchers inception" _(Launcher which starts another Launcher)_ is despised by many it remains a matter of personal preference.
But the core issue for me was that it didn't work with the Steam version nor with Linux/Proton when I tried it.

I aimed to restore the online features of *Red Alert 3* without relying on their launcher. Given how straightforward it was to re-enable these features in the client, it feels criminal to not open source this solution and share the knowledge.

## Reminder

Please be reminded that *Red Alert 3* still requires a bunch of ports to be opened to play online:

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