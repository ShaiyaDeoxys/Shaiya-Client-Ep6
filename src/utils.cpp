#include <hook.h>

static void MemWrite(void* addr, const void* data, size_t len) {
    DWORD old;
    VirtualProtect(addr, len, PAGE_EXECUTE_READWRITE, &old);
    std::memcpy(addr, data, len);
    VirtualProtect(addr, len, old, &old);
    FlushInstructionCache(GetCurrentProcess(), addr, len);
}

static void MemFillNop(void* addr, size_t len) {
    BYTE* p = reinterpret_cast<BYTE*>(addr);
    DWORD old;
    VirtualProtect(addr, len, PAGE_EXECUTE_READWRITE, &old);
    for (size_t i = 0; i < len; i++) p[i] = 0x90;
    VirtualProtect(addr, len, old, &old);
    FlushInstructionCache(GetCurrentProcess(), addr, len);
}

static void JumpRel32(void* src, void* dst, size_t len) {
    BYTE* p = reinterpret_cast<BYTE*>(src);
    intptr_t rel = (BYTE*)dst - (p + 5);
    BYTE buf[5] = { 0xE9 };
    std::memcpy(buf + 1, &rel, 4);
    MemWrite(src, buf, 5);
    if (len > 5) MemFillNop(p + 5, len - 5);
}

// Kullanýlan imza ile uyumlu minimal Hook
static inline void Hook(void* at, void* tramp, size_t len) {
    JumpRel32(at, tramp, len);
}

