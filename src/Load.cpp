#include "hook.h"


const char* interface_data_path = "data/interface";
const char* icon_data_path = "data/interface/icon";
const char* effect_data_path = "data/interface/icon";
auto icon_wing_dds = "icon_wing.dds";
auto new_dds_1 = "icon_weapon_skin.dds";




DWORD load_dds_retn = 0x59F0D9;
DWORD load_dds_jump = 0x4B4F19;
_declspec(naked) void load_dds()
{
	_asm
	{
		cmp dword ptr ds : [0x90CA1C] , 0x0
		jne load_slot_17_dds
		push 512
		push 512
		push icon_wing_dds
		push icon_data_path
		mov ecx, 0x90CA1C
		call load_interface_elements

		load_slot_17_dds :
		cmp dword ptr[alloc_new_dds_1], 00
			jne load_dds_exit
			push 512
			push 512
			push new_dds_1
			push icon_data_path
			mov ecx, offset alloc_new_dds_1
			call load_interface_elements

			load_dds_exit :
		jmp load_dds_jump
	}
}


void Load() {

	utils::Hook((LPVOID)0x4B4EF2, load_dds, 7);

}
