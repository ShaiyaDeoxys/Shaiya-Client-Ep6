#pragma once
#include <windows.h>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace utils {

	inline void MemWrite(void* addr, const void* data, size_t len) { /*...*/ }
	inline void MemFillNop(void* addr, size_t len) { /*...*/ }
	inline void JumpRel32(void* src, void* dst, size_t len) { /*...*/ }
	inline void Hook(void* at, void* tramp, size_t len) { JumpRel32(at, tramp, len); }

} 