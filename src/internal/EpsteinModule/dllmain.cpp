// dllmain.cpp — Cs2 Epstein Rage DLL Entry Point (Anti-Detection Build)
//
// Changes from detected version:
// - No MinHook (COM vtable swap for Present, VMT hook for CreateMove)
// - No code patching (thirdperson handled via SafeWrite in Present hook)
// - No console/printf in Release builds (#ifdef _DEBUG only)
// - Indirect memory R/W for sensitive addresses (viewangles, frame history)
//
#include <Windows.h>
#include <Psapi.h>
#include <thread>
#include <chrono>
#include <cstdio>
#include <cstdarg>
#include <cstring>

#include "hooks.h"
#include "menu.h"
#include "skin_changer.h"

#include "aimbot.h"
#include "game.h"

#define STB_IMAGE_IMPLEMENTATION
#include "minhook-master/include/MinHook.h"
#include "stb_image.h"

#pragma comment(lib, "Psapi.lib")

HMODULE g_hModule = nullptr;

// ---------------------------------------------------------------------------
// Startup logging.
//
// Every diagnostic in this file used to sit behind #ifdef _DEBUG, and the one
// unconditional printf went to a console that AllocConsole may well have failed
// to give us. In a Release inject that adds up to zero output, so a startup
// that dies halfway looks exactly like one that never ran.
//
// The log below is unconditional and goes to a file next to the DLL, which
// survives both a missing console and a thread that exits early.
// ---------------------------------------------------------------------------
static void Log(const char* fmt, ...)
{
    static char logPath[MAX_PATH]{};
    if (!logPath[0])
    {
        if (GetModuleFileNameA(g_hModule, logPath, MAX_PATH))
        {
            char* slash = strrchr(logPath, '\\');
            if (slash) strcpy_s(slash + 1, MAX_PATH - (slash + 1 - logPath), "epstein.log");
        }
        else
        {
            strcpy_s(logPath, "epstein.log");
        }
    }

    FILE* lf = nullptr;
    if (fopen_s(&lf, logPath, "a") != 0 || !lf)
        return;

    va_list args;
    va_start(args, fmt);
    vfprintf(lf, fmt, args);
    va_end(args);

    fputc('\n', lf);
    fclose(lf);

    // Mirrored so a debugger / DebugView also sees it.
    OutputDebugStringA(fmt);
}

void MainThread(HMODULE hModule)
{
    Log("=== MainThread start ===");

    const BOOL consoleOk = AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);
    SetConsoleTitleA("Cs2 Epstein Rage");
    Log("AllocConsole: %s", consoleOk ? "OK" : "FAILED (game already owns a console)");
    printf("[+] Injected (Anti-Detection Build)\n");

    Game::clientBase = Game::GetModuleBase(L"client.dll");
    Log("client.dll base: 0x%IX", Game::clientBase);

    Log("Waiting for client.dll / engine2.dll / scenesystem.dll ...");
    int waited = 0;
    while (!GetModuleHandleW(L"client.dll") || !GetModuleHandleW(L"engine2.dll") || !GetModuleHandleA("scenesystem.dll"))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (++waited % 50 == 0)
            Log("  still waiting (%d.%ds) client=%d engine2=%d scenesystem=%d",
                waited / 10, waited % 10,
                GetModuleHandleW(L"client.dll") != nullptr,
                GetModuleHandleW(L"engine2.dll") != nullptr,
                GetModuleHandleA("scenesystem.dll") != nullptr);
        if (waited > 600) // 60s
        {
            Log("[-] Modules never appeared — giving up");
            return;
        }
    }
    Log("All modules present");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (!Game::Init())
    {
        Log("[-] Game::Init() FAILED");
        printf("[-] Game::Init() FAILED\n");
        return;
    }
    Log("[+] Game::Init() OK (clientBase=0x%IX)", Game::clientBase);

    // === DX11 HOOKS: Vtable pointer swap in dxgi.dll ===
    if (!Hooks::SetupDXHooks())
    {
        Log("[-] Hooks::SetupDXHooks() FAILED — no menu, no ESP");
        printf("[-] DX hooks FAILED\n");
        return;
    }
    Log("[+] DX hooks OK (Present + ResizeBuffers)");



    // Initialize MinHook for CreateMove
    if (MH_Initialize() != MH_OK)
    {
        Log("[-] MH_Initialize() FAILED");
        printf("[-] MinHook init FAILED\n");
        return;
    }
    Log("[+] MinHook OK");

    // Initialize aimbot (CreateMove MinHook on signature). Non-fatal: a stale
    // CreateMove signature must not cost us the menu and the skin changer.
    Log("Aimbot::Init() -> %s", Aimbot::Init() ? "OK" : "FAILED (aimbot off, rest still runs)");

    Log("[+] Startup complete. INSERT=menu, END=unload");
    printf("[*] INSERT=menu, END=unload\n");

    // Wait for unload key
    SkinChanger::running.store(true);
    while (SkinChanger::running.load())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (GetAsyncKeyState(VK_END) & 1)
            break;
    }

    // Cleanup
    SkinChanger::running.store(false);
    Aimbot::Shutdown();
    Hooks::Shutdown();
    MH_Uninitialize();

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    FreeConsole();

    FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        g_hModule = hModule;
        CloseHandle(CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(MainThread), hModule, 0, nullptr));
        break;
    case DLL_PROCESS_DETACH:
        SkinChanger::running.store(false);
        break;
    }
    return TRUE;
}
