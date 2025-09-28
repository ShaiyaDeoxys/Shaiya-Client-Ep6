#include <hook.h>

UINT8 max_slot[] = { 20 };


float inventory_item_pos[38] = {
	24,53,		// Helmet
	24,93,		// Armor
	24,133,		// Pants
	24,173,		// Gauntlet
	24,213,		// Boots
	196,133,	// Weapon
	196,173,	// Shield
	196,213,	// Cape
	191,53,		// Amulet
	191,77,		// Ring
	215,77,		// Ring
	191,101,	// Loop
	215,101,	// Loop
	215,53,		// Mount
	168,53,	    // Pet
	157,213,	// Costume
	168,75,	    // Wing
	157,172,	// Weapon
	168,100     // Slot 1
};


DWORD inventory_item_equipment_retn = 0x5183E6;
_declspec(naked) void inventory_item_equipment()
{
	_asm {
		fld dword ptr[ebx * 8 + inventory_item_pos]
		fiadd[esi + 04]
		call set_tga_position
		fld dword ptr[ebx * 8 + inventory_item_pos + 4]
		jmp inventory_item_equipment_retn
	}
}

DWORD inventory_item_equipment_mouse_over_retn = 0x51A218;
_declspec(naked) void inventory_item_equipment_mouse_over()
{
	_asm {
		lea edi, [inventory_item_pos + 4]
		jmp inventory_item_equipment_mouse_over_retn
	}
}



DWORD slot_packet_retn = 0x4E8265;
_declspec(naked) void slot_packet()
{
	_asm
	{
		movzx eax, al
		cmp eax, 151
		je slot17

		cmp eax, 152
		je slot18

		cmp eax, 153
		je slot19
		jmp slot_packet_exit

		slot17 :
		mov dl, 17
			mov al, dl
			ret 4
			jmp slot_packet_exit

			slot18 :
		mov dl, 18
			mov al, dl
			ret 4
			jmp slot_packet_exit

			slot19 :
		mov dl, 19
			mov al, dl
			ret 4


			slot_packet_exit :
			dec eax
			cmp eax, 0x95
			jmp slot_packet_retn
	}
}

DWORD fix_visual_bug_retn = 0x59F0D9;
_declspec(naked) void fix_visual_bug()
{
	_asm
	{
		cmp edi, 19
		jle visual_bug_exit
		xor edx, edx
		jmp fix_visual_bug_retn

		visual_bug_exit :
		movzx edx, byte ptr[edi + esi + 0x1A8]
			jmp fix_visual_bug_retn
	}
}

DWORD load_icon_retn = 0x4B5355;
_declspec(naked) void load_icon()
{
	_asm
	{
		cmp eax, 121
		jne load_slot_17_icon
		mov eax, 0x90CA1C
		ret 8

		load_slot_17_icon:
			cmp eax, 151
			je dds_loading
			cmp eax, 152
			je dds_loading
			cmp eax, 153
			je dds_loading
			jmp load_icon_exit

			dds_loading :
		mov eax, offset alloc_new_dds_1
			ret 8

			load_icon_exit :
			jmp load_icon_retn
	}
}

DWORD load_dds_size_retn = 0x4B53EF;
DWORD load_dds_size_je = 0x4B5406;

__declspec(naked) void load_dds_size()
{
	__asm
	{
		cmp al, 30
		je bigger
		cmp al,95
		je bigger
		cmp al, 151
		je bigger
		cmp al, 152
		je bigger
		cmp al, 153
		je bigger
		cmp al, 154
		je bigger
		cmp al, 155
		je bigger


		mov eax, [esi]
		add eax, -0x19
		jmp load_dds_size_retn

			bigger :
	jmp load_dds_size_je

	}
}



void Slots()
{
	utils::Hook((LPVOID)0x4E825C, slot_packet, 9);
	utils::Hook((LPVOID)0x59F0D1, fix_visual_bug, 8);
	utils::Hook((LPVOID)0x4B5348, load_icon, 5);
	utils::Hook((LPVOID)0x4B53EA, load_dds_size, 5);
	utils::JumpRel32((LPVOID)0x004B53C3, (LPVOID)0x004B53EA, 8);
	utils::MemWrite((LPVOID)(0x51909D + 2), max_slot, 1);
	utils::MemWrite((LPVOID)(0x518596 + 2), max_slot, 1);
	utils::MemWrite((LPVOID)(0x51A26E + 2), max_slot, 1);
	utils::MemWrite((LPVOID)(0x517F8B + 1), max_slot, 1);
	utils::MemWrite((LPVOID)(0x4BCACC + 1), max_slot, 1);

	utils::MemWrite((LPVOID)(0x4E8255 + 1), max_slot, 1);
	utils::MemWrite((LPVOID)(0x4FEC1A + 1), max_slot, 1);
	utils::MemWrite((LPVOID)(0x4F4195 + 2), max_slot, 1); // Comes from itemmall fix

	utils::Hook((LPVOID)0x5183D0, inventory_item_equipment, 7);
	utils::Hook((LPVOID)0x51A212, inventory_item_equipment_mouse_over, 6);

}

