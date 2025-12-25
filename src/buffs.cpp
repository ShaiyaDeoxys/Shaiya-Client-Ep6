#include <hook.h>

namespace {
    // INI
    constexpr const char* INI_PATH = ".\\CONFIG.ini";
    constexpr const char* INI_SECTION = "BUFF";

    // Oyun adresleri / dönüşler
    constexpr DWORD ADDR_MOUSE_X = 0x7C3C0C;
    constexpr DWORD ADDR_MOUSE_Y = 0x7C3C10;
    constexpr DWORD RET_MOUSE_TR = 0x004D73F8; 
    constexpr DWORD RET_NEWLOC_TR = 0x004D7509; 

    // Konum durumu
    volatile int gLocX = 0;
    volatile int gLocY = 0;
    int sSavedX = INT_MIN, sSavedY = INT_MIN;

    // --- INI yardımcıları ---
    inline void LoadIni() {
        gLocX = GetPrivateProfileIntA(INI_SECTION, "LOCATION_X", 0, INI_PATH);
        gLocY = GetPrivateProfileIntA(INI_SECTION, "LOCATION_Y", 0, INI_PATH);
        sSavedX = gLocX; sSavedY = gLocY;
    }

    inline void WriteIniInt(const char* key, int v) {
        char buf[16]; wsprintfA(buf, "%d", v);
        WritePrivateProfileStringA(INI_SECTION, key, buf, INI_PATH);
    }

    inline void SaveIfChanged() {
        if (gLocX != sSavedX || gLocY != sSavedY) {
            WriteIniInt("LOCATION_X", gLocX);
            WriteIniInt("LOCATION_Y", gLocY);
            sSavedX = gLocX; sSavedY = gLocY;
        }
    }

    // --- Trampolinler ---
    __declspec(naked) void MouseTramp() {
        __asm {
            mov  edi, dword ptr[gLocX]
            mov  ebx, dword ptr[gLocY]
            jmp  RET_MOUSE_TR
        }
    }

    __declspec(naked) void NewLocationTramp() {
        __asm {
            mov  edi, dword ptr[gLocX]
            jmp  RET_NEWLOC_TR
        }
    }


    DWORD WINAPI Worker(LPVOID) {
        for (;;) {
            const SHORT sh = GetAsyncKeyState(VK_SHIFT);
            const SHORT rb = GetAsyncKeyState(VK_RBUTTON);
            if ((sh & 0x8000) && (rb & 0x8000)) {
                gLocX = *reinterpret_cast<int*>(ADDR_MOUSE_X);
                gLocY = *reinterpret_cast<int*>(ADDR_MOUSE_Y);
                SaveIfChanged();
                Sleep(150); 
            }
            else {
                Sleep(15);
            }
        }
        // not reached
        // return 0;
    }
} // namespace


void Buff_Icons() {
    LoadIni();
    utils::Hook((LPVOID)0x004D73ED, MouseTramp, 11);
    utils::Hook((LPVOID)0x004D7503, NewLocationTramp, 6);
    CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
}
