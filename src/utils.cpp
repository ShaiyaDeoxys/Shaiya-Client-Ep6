#include <hook.h>



namespace utils {

    void MemWrite(void* addr, const void* data, size_t len) {
        DWORD old;
        VirtualProtect(addr, len, PAGE_EXECUTE_READWRITE, &old);
        std::memcpy(addr, data, len);
        VirtualProtect(addr, len, old, &old);
        FlushInstructionCache(GetCurrentProcess(), addr, len);
    }

    void MemFillNop(void* addr, size_t len) {
        BYTE* p = reinterpret_cast<BYTE*>(addr);
        DWORD old;
        VirtualProtect(addr, len, PAGE_EXECUTE_READWRITE, &old);
        for (size_t i = 0; i < len; i++) p[i] = 0x90;
        VirtualProtect(addr, len, old, &old);
        FlushInstructionCache(GetCurrentProcess(), addr, len);
    }

    void JumpRel32(void* src, void* dst, size_t len) {
        BYTE* p = reinterpret_cast<BYTE*>(src);
        intptr_t rel = (BYTE*)dst - (p + 5);
        BYTE buf[5] = { 0xE9 };
        std::memcpy(buf + 1, &rel, 4);
        MemWrite(src, buf, 5);
        if (len > 5) MemFillNop(p + 5, len - 5);
    }

    void Hook(void* at, void* tramp, size_t len) {
        JumpRel32(at, tramp, len);
    }

} 

char alloc_new_dds_1[16];

DWORD set_tga_position = 0x631BE0;
DWORD load_interface_elements = 0x57B560;