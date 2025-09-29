#pragma once
#include <windows.h>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace utils 
{
	void MemWrite(void* addr, const void* data, size_t len);
	void MemFillNop(void* addr, size_t len);
	void JumpRel32(void* src, void* dst, size_t len);
	void Hook(void* at, void* tramp, size_t len);
	void PatchByte(void* addr, BYTE value);
}

extern char alloc_new_dds_1[16];

extern DWORD load_interface_elements;
extern DWORD set_tga_position;