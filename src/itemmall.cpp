#include <hook.h>

// ===== Addresses (same as your working build) =====
#define ADDR_HOOK_TYPEWRITE      0x004F4224 
#define ADDR_HOOK_REFRESHBEFORE  0x004F4319 

#define RET1_JL                  0x004F4170 
#define RET1_END                 0x004F42F8 

#define ADDR_HELDTYPE            0x0090E3FD  // byte: 1..15
#define ADDR_FACTION             0x022AA816  // byte: 0=Light, 1=Fury
#define ADDR_COSTYPE             0x0090E605  // byte
#define ADDR_COSID               0x0090E606  // byte

// ===== Preview layout =====
#define OFF_PREVIEW_BASE  0xE34C
#define OFF_TYPE          0x01A8
#define OFF_ID            0x01B9
#define OFF_WPN_TYPE      0x01AD   
#define OFF_WPN_ID        0x01BE   

// ===== Mapping tables (index = 1..15) =====
__declspec(align(1)) static unsigned char g_mapLight[16] =
{ 0, 45,46,47,48,49,51,53,55,57,58,64,60,62,64,65 };
__declspec(align(1)) static unsigned char g_mapFury[16] =
{ 0, 45,46,47,48,49,52,54,56,57,58,59,61,63,59,65 };

__declspec(naked) void weapon_skin_map()
{
    __asm {
        // Original: write TYPE to current slot
        movzx edx, byte ptr[esp + 0x30]                 // DL = type
        mov   ecx, dword ptr[esi + OFF_PREVIEW_BASE]    // base
        movzx eax, bl                                   // slot
        mov   byte ptr[ecx + eax + OFF_TYPE], dl        // [+1A8 + slot] = type

        // Only type 151 → map & write to weapon-skin slot
        cmp   dl, 151
        jne   after_force

        // Keep ID=X before pushes
        mov   al, byte ptr[esp + 0x18]                  // AL = X
        movzx edi, al                                   // EDI = X

        push  ebx
        push  esi

        // heldType := [ADDR_HELDTYPE]  (1..15)
        mov   eax, ADDR_HELDTYPE
        mov   bl, byte ptr[eax]
        cmp   bl, 1
        jb    restore
        cmp   bl, 15
        ja    restore
        movzx edx, bl                                   // index

        // faction := [ADDR_FACTION] (0=Light/1=Fury)
        mov   eax, ADDR_FACTION
        mov   al, byte ptr[eax]

        // mapped TYPE → DL
        test  al, al
        jnz   use_fury
        lea   ecx, g_mapLight
        mov   dl, byte ptr[ecx + edx]
        jmp   have_type
    use_fury:
        lea   ecx, g_mapFury
        mov   dl, byte ptr[ecx + edx]
    have_type:
        // Write mapped TYPE/ID to weapon-skin slot
        mov   ecx, dword ptr[esi + OFF_PREVIEW_BASE]
        mov   eax, edi                                  // AL = X
        mov   byte ptr[ecx + OFF_WPN_TYPE], dl
        mov   byte ptr[ecx + OFF_WPN_ID],   al

    restore:
        pop   esi
        pop   ebx

    after_force:
        // Original: write TYPEID to current slot & loop
        movzx edx, byte ptr[esp + 0x18]                 // DL = typeId
        mov   ecx, dword ptr[esi + OFF_PREVIEW_BASE]
        mov   byte ptr[ecx + eax + OFF_ID], dl          // [+1B9 + slot] = id

        mov   byte ptr[esp + 0x17], 1
        mov   eax, dword ptr[esp + 0x1C]
        inc   eax
        add   ebp, 4
        cmp   eax, 18
        mov   dword ptr[esp + 0x1C], eax
        jl    short GO_JL

        mov   eax, RET1_END
        jmp   eax
    GO_JL:
        mov   eax, RET1_JL
        jmp   eax
    }
}

#define ADDR_SKIP_CHAR          0x004F50C1
#define ADDR_SKIP_CHAR_JE       0x004F50CC
#define LEN_SKIP_CHAR           7

__declspec(naked) void skip_char_view_tab1()
{
    __asm {
        // newmem:
        // cmp [ebp+2F10],1
        // mov ecx, ebp
        // je 004F50CC
        cmp dword ptr [ebp + 0x2F10], 1
        mov ecx, ebp
        jne no_skip
        mov eax, ADDR_SKIP_CHAR_JE
        jmp eax

    no_skip:
        // originalcode:
        // cmp [ebp+2F10],5
        cmp dword ptr [ebp + 0x2F10], 5

        // exit: jmp returnhere (004F50C1 + 7)
        mov eax, ADDR_SKIP_CHAR
        add eax, LEN_SKIP_CHAR
        jmp eax
    }
}

#define ADDR_SHOW_PET           0x004F419E
#define ADDR_SHOW_PET_JE        0x004F41A7
#define LEN_SHOW_PET            7

__declspec(naked) void show_pet_tab1()
{
    __asm {
        // newmem:
        // cmp [esi+2F10],1
        // je 004F41A7
        cmp dword ptr [esi + 0x2F10], 1
        jne no_pet
        mov eax, ADDR_SHOW_PET_JE
        jmp eax

    no_pet:
        // originalcode:
        // cmp [esi+2F10],5
        cmp dword ptr [esi + 0x2F10], 5

        // exit: jmp returnhere (004F419E + 7)
        mov eax, ADDR_SHOW_PET
        add eax, LEN_SHOW_PET
        jmp eax
    }
}


#define ADDR_SHOW_MOUNT         0x004F426C
#define ADDR_MOUNT_SUCCESS      0x004F428C
#define ADDR_MOUNT_ELSE         0x004F4272
#define ADDR_CALL_MOUNT         0x0043A060
#define LEN_SHOW_MOUNT          6

__declspec(naked) void show_mount_tab1()
{
    __asm {
        // originalcode (ilk satır CE'de de böyle başlıyor)
        mov [esi + 0xE350], eax

        // Tab1 ise mount akışını çağır → 004F428C
        cmp dword ptr [esi + 0x2F10], 1
        jne jnepart

        mov ecx, [edi + 0x90]
        xor edx, edx
        push ecx
        mov byte ptr [esp + 0x2C], 0xFF   // -1
        push edx
        mov ecx, eax
        mov eax, ADDR_CALL_MOUNT
        call eax

        mov eax, ADDR_MOUNT_SUCCESS
        jmp eax

    jnepart:
        // Tab1 değilse default yol → 004F4272
        cmp dword ptr [esi + 0x2F10], 5
        mov eax, ADDR_MOUNT_ELSE
        jmp eax
    }
}

void Itemmall()
{
    // HOOK #1 — type write + weapon skin map
    utils::Hook((LPVOID)ADDR_HOOK_TYPEWRITE, weapon_skin_map, 5);

    utils::PatchByte((void*)0x004F4210, 0xEB);

    // HOOK #2 — skip_char_view_tab1
    utils::Hook((LPVOID)ADDR_SKIP_CHAR,  skip_char_view_tab1,  LEN_SKIP_CHAR);

    // HOOK #3 — show_pet_tab1
    utils::Hook((LPVOID)ADDR_SHOW_PET,   show_pet_tab1,       LEN_SHOW_PET);

    // HOOK #4 — show_mount_tab1
    utils::Hook((LPVOID)ADDR_SHOW_MOUNT, show_mount_tab1,     LEN_SHOW_MOUNT);
}
