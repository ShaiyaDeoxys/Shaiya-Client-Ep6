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

__declspec(naked) void tramp_weapon_skin_map()
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


// ================== ID:26 — skip_char_view_tab1 ==================
// CT mantığı:
// if (tab==1 || tab==5) { call 004F38D0; jmp 004F50D8; }
// else { jmp 004F50D3; }
#define ADDR_SKIP_CHAR_PATCH    0x004F50C1  // CE patch noktası
#define LEN_SKIP_CHAR_PATCH     7           // CE: jmp + 2x NOP
#define ADDR_SKIP_ELSE_JMP      0x004F50D3  // Tab!=1 && Tab!=5 → buraya
#define ADDR_SKIP_DO_JMP        0x004F50D8  // call sonrası buraya
#define ADDR_SKIP_CALL          0x004F38D0  // CE'deki call hedefi

__declspec(naked) void tramp_skip_char_view_tab1_v2()
{
    __asm {
        cmp dword ptr[ebp + 0x2F10], 1
        mov ecx, ebp
        je  do_part
        cmp dword ptr[ebp + 0x2F10], 5
        mov ecx, ebp
        je  do_part

        // jmp 004F50D3 (register kirletmeden)
        push ADDR_SKIP_ELSE_JMP
        ret

        do_part :
        // call 004F38D0  (MSVC: önce EAX'a al)
        mov eax, ADDR_SKIP_CALL
            call eax

            // jmp 004F50D8
            push ADDR_SKIP_DO_JMP
            ret
    }
}

// ================== ID:27 — show_pet_tab1 ==================
// CT mantığı:
// if (tab==1 || tab==5) jmp 004F41A7; else jmp 004F4203;
#define ADDR_SHOW_PET_PATCH     0x004F419E  // CE patch noktası
#define LEN_SHOW_PET_PATCH      7           // CE: jmp + 2x NOP
#define ADDR_SHOW_PET_JE_TGT    0x004F41A7  // Tab==1 || Tab==5 → buraya
#define ADDR_SHOW_PET_ELSE_JMP  0x004F4203  // Diğerleri → buraya

__declspec(naked) void tramp_show_pet_tab1_v2()
{
    __asm {
        cmp dword ptr[esi + 0x2F10], 1
        je  do_pet
        cmp dword ptr[esi + 0x2F10], 5
        je  do_pet

        // jmp 004F4203
        push ADDR_SHOW_PET_ELSE_JMP
        ret

        do_pet :
        // jmp 004F41A7
        push ADDR_SHOW_PET_JE_TGT
            ret
    }
}

// ================== ID:13 — show_mount_tab1 ==================
// CT mantığı (özet):
// mov [esi+E350],eax
// if (tab==1) load_mount();
// else if (tab==5) jmp 004F4272;
// else load_mount();
//
// load_mount():
//   mov ecx,[edi+90]; xor edx,edx; push ecx; [esp+2C]=0xFF; push edx; mov ecx,eax; call 0043A060; jmp 004F428C;
#define ADDR_SHOW_MOUNT_PATCH   0x004F426C  // CE patch noktası
#define LEN_SHOW_MOUNT_PATCH    6           // CE: jmp + 1x NOP
#define ADDR_MOUNT_CALL         0x0043A060  // CE'deki call
#define ADDR_MOUNT_SUCCESS_JMP  0x004F428C  // yükleme sonrası
#define ADDR_MOUNT_TAB5_JMP     0x004F4272  // Tab==5 → buraya

__declspec(naked) void tramp_show_mount_tab1_v2()
{
    __asm {
        // CE: originalcode ilk satır
        mov[esi + 0xE350], eax

        // Tab==1 → load_mount
        cmp dword ptr[esi + 0x2F10], 1
        je  load_mount
        // Tab==5 → 004F4272
        cmp dword ptr[esi + 0x2F10], 5
        je  tab5_path


        load_mount :
            mov ecx, [edi + 0x90]
            xor edx, edx
            push ecx
            mov byte ptr[esp + 0x2C], 0xFF   // -1
            push edx
            mov ecx, eax


            mov eax, ADDR_MOUNT_CALL
            call eax

            // jmp 004F428C
            push ADDR_MOUNT_SUCCESS_JMP
            ret

            tab5_path :
        // jmp 004F4272
        push ADDR_MOUNT_TAB5_JMP
            ret
    }
}

void Itemmall()
{

    utils::Hook((LPVOID)ADDR_HOOK_TYPEWRITE, tramp_weapon_skin_map, 5);
    utils::PatchByte((void*)0x004F4210, 0xEB);
    utils::Hook((LPVOID)ADDR_SKIP_CHAR_PATCH, tramp_skip_char_view_tab1_v2, LEN_SKIP_CHAR_PATCH);
    utils::Hook((LPVOID)ADDR_SHOW_PET_PATCH, tramp_show_pet_tab1_v2, LEN_SHOW_PET_PATCH);
    utils::Hook((LPVOID)ADDR_SHOW_MOUNT_PATCH, tramp_show_mount_tab1_v2, LEN_SHOW_MOUNT_PATCH);
}
