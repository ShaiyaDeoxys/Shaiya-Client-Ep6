#include <hook.h>

extern "C"  __declspec(dllexport) void __cdecl DLLMain()
{
   StartHooks();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        DLLMain();

        break;
    case DLL_PROCESS_DETACH:
        StopHooks();
        break;
    }
    return TRUE;
}
