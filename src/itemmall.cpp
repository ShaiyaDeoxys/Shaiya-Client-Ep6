#include <hook.h>


DWORD char_view_jmp = 0x4F419E;
DWORD char_view_jne = 0x4F4198;

static __declspec(naked) void char_view()
{
    __asm {

        cmp byte ptr[esp + 0x30], 151
        jne jne_part
        mov bl, 0x05
        jmp char_view_jmp

        jne_part:
        mov bl, al
        cmp bl, 0x14
        jmp char_view_jne


    }
}

DWORD weapon_skin_retn = 0x4F4239;
DWORD weapon_skin_jl = 0x4F4170;
DWORD weapon_skin_jmp = 0x4F42F8;

static __declspec(naked) void weapon_skin() {
    __asm {


        cmp dl, 151
            je jepart
            mov[eax + ecx + 0x1A8], dl
        jmp weapon_skin_retn

            jepart:
            mov[eax + ecx + 0x1DA], dl
            movzx edx, byte ptr[esp + 0x18]
            mov ecx, [esi + 0xE34C]
            mov[eax + ecx + 0x1B9], dl
            mov byte ptr[esp + 0x17], 0x01
            mov eax, [esp + 0x1C]
            inc eax
            add ebp, 0x4
            cmp eax, 0x18
            mov[esp + 0x1C], eax
            jl jl_part
            jmp weapon_skin_jmp


                jl_part:
            jmp weapon_skin_jl
    }
}


void Itemmall() 
{

    utils::Hook((void*)0x4F4193, char_view, 5);
    utils::Hook((void*)0x4F4232, weapon_skin, 7);
}

