#include "../Includes.h"
#include "InternalMain.h"

// -----------------------------------------------------------------------
// Entry point of SkinCore.dll.
//
// DllMain runs under the loader lock, so it must not do real work here —
// it only spawns the worker thread and returns immediately.
// -----------------------------------------------------------------------

static DWORD WINAPI WorkerThread(LPVOID lpParameter)
{
    InternalCore::Run(reinterpret_cast<HMODULE>(lpParameter));
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);

        const HANDLE hThread = CreateThread(nullptr, 0, WorkerThread, hModule, 0, nullptr);
        if (hThread)
            CloseHandle(hThread);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        g_Globals.m_bIsUnloading = true;
    }

    return TRUE;
}
