/* 
From: https://github.com/lanyizi/BegoneCrashers
All credits to lanyizi
*/
extern "C" {

    __declspec(naked) void WallCrash_patch0()
    {
        //  mov     ecx, [edx + 374h]
        //  mov     edx, [esi + 374h]
        //  mov     eax, [edx + 144h] // This might crash
        //  mov     edi, [ecx + 144h] // This might crash
        __asm
        {
            // availaible registers: edi, ebp, eax, ecx

            mov     edi, dword ptr[esi + 0x374];
            test    edi, edi; // check if it's null
            // if it's null, jump away to prevent crash
            jz      failure;

            mov     edi, dword ptr[edx + 0x374];
            test    edi, edi; // check if it's null
            // if it's null, jump away to prevent crash
            jz      failure;

            mov     edi, dword ptr[ebx];
            mov     ebp, dword ptr[esp + (0x5C - 0x48)];

            // Now we are sure it won't crash.
            // Go back to game code which loads [edx + 144h] and [ecx + 144h]
            // 0x54EA8E is Steam version address.
            // Origin version address: 0x59004E
            push    0x54EA8E; // go back to game code
            ret;
        failure:
            // We think the game is going crash
            // We jump away to prevent executing the crashing code.
            // 0x54EB32 is Steam version address.
            // Origin version address: 0x5900F2
            push    0x54EB32; // jump away
            ret;
        }
    }

    __declspec(naked) void WallCrash_patch0b()
    {
        // Fix crash for Origin version at 0x590048

        //  mov     ecx, [edx + 374h]
        //  mov     edx, [esi + 374h]
        //  mov     eax, [edx + 144h] // This might crash
        //  mov     edi, [ecx + 144h] // This might crash
        __asm
        {
            // availaible registers: edi, ebp, eax, ecx

            mov     edi, dword ptr[esi + 0x374];
            test    edi, edi; // check if it's null
            // if it's null, jump away to prevent crash
            jz      failure;

            mov     edi, dword ptr[edx + 0x374];
            test    edi, edi; // check if it's null
            // if it's null, jump away to prevent crash
            jz      failure;

            mov     edi, dword ptr[ebx];
            mov     ebp, dword ptr[esp + (0x5C - 0x48)];

            // Now we are sure it won't crash.
            // Go back to game code which loads [edx + 144h] and [ecx + 144h]
            // 0x59004E is Origin version address.
            // Steam version address: 0x54EA8E
            push    0x59004E; // go back to game code
            ret;
        failure:
            // We think the game is going crash
            // We jump away to prevent executing the crashing code.
            // 0x5900F2 is Origin version address.
            // Steam version address: 0x54EB32
            push    0x5900F2; // jump away
            ret;
        }
    }

    __declspec(naked) void WallCrash_patch1()
    {
        // Steam Address: 0x81D1F6
        // Origin Address: 0x85B386
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

}