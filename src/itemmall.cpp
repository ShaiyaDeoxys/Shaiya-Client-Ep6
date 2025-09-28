#include <hook.h>

// ===== Addresses (same as your working build) =====
#define ADDR_HOOK_TYPEWRITE      0x004F4224
#define ADDR_HOOK_REFRESHBEFORE  0x004F4319

#define RET1_JL                  0x004F4170
#define RET1_END                 0x004F42F8

#define REFRESH_CALL_ADDR        0x00418BB0
#define REFRESH_CONT_ADDR        0x004F4324

#define ADDR_HELDTYPE            0x0090E3FD  // byte: 1..15
#define ADDR_FACTION             0x022AA816  // byte: 0=Light, 1=Fury
#define ADDR_COSTYPE             0x0090E605  // byte
#define ADDR_COSID               0x0090E606  // byte

// ===== Preview layout =====
#define OFF_PREVIEW_BASE  0xE34C
#define OFF_TYPE          0x01A8
#define OFF_ID            0x01B9
#define OFF_WPN_TYPE      0x01AD   // 0x1A8 + 5
#define OFF_WPN_ID        0x01BE   // 0x1B9 + 5
#define OFF_COS_TYPE      0x01B7   // 0x1A8 + 0x0F
#define OFF_COS_ID        0x01C8   // 0x1B9 + 0x0F

// ===== Mapping tables (index = 1..15) =====
__declspec(align(1)) static unsigned char g_mapLight[16] =
{ 0, 45,46,47,48,49,51,53,55,57,58,64,60,62,64,65 };
__declspec(align(1)) static unsigned char g_mapFury[16] =
{ 0, 45,46,47,48,49,52,54,56,57,58,59,61,63,59,65 };

// ===== NEW: one–shot gate for writing equipped costume during weapon-skin preview =====
static volatile BYTE g_writeEquippedCostumeOnce = 0;


__declspec(naked) void weapon_skin_map()
{
    __asm {
        // Original: write TYPE to current slot
        movzx edx, byte ptr[esp + 0x30]                 // DL = type
        mov   ecx, dword ptr[esi + OFF_PREVIEW_BASE]    // base
        movzx eax, bl                                  // slot
        mov   byte ptr[ecx + eax + OFF_TYPE], dl          // [+1A8 + slot] = type

        // Handle only type 151
        cmp   dl, 151
        jne   after_force

        // NEW: arm the "write equipped costume once" flag
        mov   dword ptr[g_writeEquippedCostumeOnce], 1

        // Keep ID=X before pushes
        mov   al, byte ptr[esp + 0x18]                 // AL = X
        movzx edi, al                                  // EDI = X

        push  ebx
        push  esi

        // heldType := [ADDR_HELDTYPE]  (1..15)
        mov   eax, ADDR_HELDTYPE
        mov   bl, byte ptr[eax]
        cmp   bl, 1
        jb    restore
        cmp   bl, 15
        ja    restore
        movzx edx, bl                                  // index

        // faction := [ADDR_FACTION] (0=Light/1=Fury)
        mov   eax, ADDR_FACTION
        mov   al, byte ptr[eax]

        // mapped TYPE → DL
        test  al, al
        jnz   use_fury
        lea   ecx, g_mapLight
        mov   dl, byte ptr[ecx + edx]
        jmp   have_type
        use_fury :
        lea   ecx, g_mapFury
            mov   dl, byte ptr[ecx + edx]
            have_type :

            // Write mapped TYPE/ID to weapon slot (+5)
            mov   ecx, dword ptr[esi + OFF_PREVIEW_BASE]
            mov   eax, edi                                 // AL = X
            mov   byte ptr[ecx + OFF_WPN_TYPE], dl
            mov   byte ptr[ecx + OFF_WPN_ID], al

            restore :
        pop   esi
            pop   ebx

            after_force :
        // Original: write TYPEID to current slot & loop
        movzx edx, byte ptr[esp + 0x18]                 // DL = typeId
            mov   ecx, dword ptr[esi + OFF_PREVIEW_BASE]
            mov   byte ptr[ecx + eax + OFF_ID], dl            // [+1B9 + slot] = id

            mov   byte ptr[esp + 0x17], 1
            mov   eax, dword ptr[esp + 0x1C]
            inc   eax
            add   ebp, 4
            cmp   eax, 18
            mov   dword ptr[esp + 0x1C], eax
            jl    short GO_JL

            mov   eax, RET1_END
            jmp   eax
            GO_JL :
        mov   eax, RET1_JL
            jmp   eax
    }
}


__declspec(naked) void costume_refresh()
{
    __asm {
        pushfd
        pushad

        // Gate: only write equipped costume if a 151 flow just occurred
        cmp   dword ptr[g_writeEquippedCostumeOnce], 1
        jne   skip_costume_write

        mov   edx, dword ptr[esi + OFF_PREVIEW_BASE]    // base

        // Costume ID -> [+1B9 + 0x0F]
        mov   eax, ADDR_COSID
        mov   al, byte ptr[eax]
        test  al, al
        jz    skip_id
        mov   byte ptr[edx + OFF_COS_ID], al
        skip_id :

        // Costume TYPE -> [+1A8 + 0x0F]
        mov   eax, ADDR_COSTYPE
            mov   bl, byte ptr[eax]
            test  bl, bl
            jz    done_costume
            mov   byte ptr[edx + OFF_COS_TYPE], bl
            done_costume :
        // one-shot: clear the gate now so costume shop can preview freely
        mov   dword ptr[g_writeEquippedCostumeOnce], 0

            skip_costume_write :
            popad
            popfd

            // Original: mov ecx,[esi+E34C]; call refresh; jmp continue
            mov   ecx, dword ptr[esi + OFF_PREVIEW_BASE]
            mov   eax, REFRESH_CALL_ADDR
            call  eax
            mov   eax, REFRESH_CONT_ADDR
            jmp   eax
    }
}


void Itemmall()
{
    utils::Hook((LPVOID)ADDR_HOOK_TYPEWRITE, weapon_skin_map, 5);
    utils::Hook((LPVOID)ADDR_HOOK_REFRESHBEFORE, costume_refresh, 6);
}
