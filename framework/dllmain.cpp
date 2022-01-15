// Generated C++ file by Il2CppInspector - http://www.djkaty.com - https://github.com/djkaty
// DLL entry point

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "il2cpp-init.h"
#include "main.h"
#include <Hooks/hooks.h>
#include "Log/cheatlog.h"

// Hooking
#include <../kiero/kiero.h>
#include <../kiero/minhook/include/MinHook.h>

// DLL entry point
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    DisableThreadLibraryCalls(hModule);
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        init_il2cpp();
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainThread, NULL, 0, NULL);
        break;
    case DLL_PROCESS_DETACH:
        kiero::shutdown();

        MH_DisableHook(MH_ALL_HOOKS);
        MH_RemoveHook(MH_ALL_HOOKS);

        MH_Uninitialize();

        FreeConsole();
        break;
    }
    return TRUE;
}