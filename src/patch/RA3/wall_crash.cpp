/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.

Based on https://github.com/lanyizi/BegoneCrashers
All credits to lanyizi
*/

#include "wall_crash.h"

__declspec(naked) void WallCrash_patch_0(){
    //  mov     ecx, [edx + 374h]
    //  mov     edx, [esi + 374h]
    //  mov     eax, [edx + 144h] // This might crash
    //  mov     edi, [ecx + 144h] // This might crash
    __asm
    {
        // Availaible registers: edi, ebp, eax, ecx

        // Reads the return address which is the base address + 5 due to the jmp instruction.
        mov     eax, dword ptr[esp]
        sub     eax, 5  // Gets back to the original base address

        mov     edi, dword ptr[esi + 0x374];
        test    edi, edi; // check if it's null
        jz      failure;  // if it's null, jump away to prevent crash

        mov     edi, dword ptr[edx + 0x374];
        test    edi, edi; // check if it's null
        jz      failure;  // if it's null, jump away to prevent crash

        mov     edi, dword ptr[ebx];
        mov     ebp, dword ptr[esp + (0x5C - 0x48)];

        // Now we are sure it won't crash.
        // Go back to game code which loads [edx + 144h] and [ecx + 144h]
        // base address + 0x06
        add     eax, 0x06
        push    eax
        ret

     failure :
        // We think the game is going crash
        // We jump away to prevent executing the crashing code.
        // base address + 0xAA
        add     eax, 0xAA
        push    eax
        ret
    }
}

__declspec(naked) void WallCrash_patch_1(){
    __asm
    {
        mov     ecx, dword ptr[ebx + 0x374];
        mov     eax, dword ptr[edi + 0x374];
        // let edx be 0 or [ecx + 0x144], only if ecx is not null
        test    ecx, ecx; // check ecx
        mov     edx, 0;
        jz      afterEdx; // skip next instruction to prevent crash
        mov     edx, dword ptr[ecx + 0x144]; // This might crash
    afterEdx:
        // let ecx be 0 or [eax + 0x144], only if eax is not null
        test    eax, eax; // check eax
        mov     ecx, 0;
        jz      afterEcx; // skip next instruction to prevent crash
        mov     ecx, dword ptr[eax + 0x144]; // This might crash
    afterEcx:
        cmp     ecx, edx;
        pop     edi;
        sbb     eax, eax;
        pop     esi;
        neg     eax;
        pop     ebx;
        add     esp, 8;
        retn    8;
    }
}