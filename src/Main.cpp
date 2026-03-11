#include "Includes.h"

FILE* m_pConsoleStream = nullptr;
std::ofstream m_ofsFile{};

bool ConsoleAttach(const char* szConsoleTitle)
{
    if (!AllocConsole())
        return false;

    AttachConsole(ATTACH_PARENT_PROCESS);

    FILE* fp;
    freopen_s(&fp, X("CONIN$"),  X("r"), stdin);
    freopen_s(&fp, X("CONOUT$"), X("w"), stdout);
    freopen_s(&fp, X("CONOUT$"), X("w"), stderr);

    if (!SetConsoleTitleA(szConsoleTitle))
        return false;

    return true;
}

bool DetachConsole()
{
    FILE* fp;
    freopen_s(&fp, X("NUL"), X("r"), stdin);
    freopen_s(&fp, X("NUL"), X("w"), stdout);
    freopen_s(&fp, X("NUL"), X("w"), stderr);
    return FreeConsole() != 0;
}

void SetThreadPriorityWrapper()
{
    HANDLE hThread = GetCurrentThread();
    if (hThread)
    {
        int nPri = GetThreadPriority(hThread);
        if (nPri == THREAD_PRIORITY_ERROR_RETURN)
            throw std::runtime_error(X("failed to get thread priority"));
        if (nPri != THREAD_PRIORITY_HIGHEST)
            SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
        CloseHandle(hThread);
    }
    else
        throw std::runtime_error(X("failed to set thread priority"));
}

// -----------------------------------------------------------------------
void EntityThread()
{
    SetThreadPriorityWrapper();
    while (!g_Globals.m_bIsUnloading)
    {
        if (!g_Utilities.IsInGame())
        {
            g_Utilities.Sleep(3000.f);
            continue;
        }
        EntityList::m_mtxEntities.lock();
        EntityList::UpdateEntities();
        EntityList::m_mtxEntities.unlock();
        g_Utilities.Sleep(INTERVAL_PER_TICK * 1000.0f);
    }
}

// -----------------------------------------------------------------------
void RenderThread()
{
    SetThreadPriorityWrapper();
    while (!g_Globals.m_bIsUnloading)
    {
        Draw::ClearDrawData();

        if (!g_Memory.IsWindowInForeground(X("Counter-Strike 2")))
        {
            Draw::SwapDrawData();
            g_Utilities.Sleep(1000.0f);
            continue;
        }

        std::unique_lock lockEntityGuard(EntityList::m_mtxEntities);
        std::vector<EntityObject_t> vecEntities;
        vecEntities.assign(EntityList::m_vecEntities.begin(), EntityList::m_vecEntities.end());
        lockEntityGuard.unlock();
        lockEntityGuard.release();

        if (Window::m_bInitialized)
        {
            // ===== ESP =====
            if (CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bEnableVisuals))
            {
                for (EntityObject_t& object : vecEntities)
                {
                    if (object.m_eType != EEntityType::ENTITY_PLAYER) continue;

                    CCSPlayerController* pController = reinterpret_cast<CCSPlayerController*>(object.m_pEntity);
                    if (!pController || pController->m_bIsLocalPlayerController()) continue;

                    C_CSPlayerPawn* pPawn = reinterpret_cast<C_CSPlayerPawn*>(pController->m_hPawn().Get());
                    if (!pPawn || !pPawn->IsAlive()) continue;

                    ESP::RenderPlayer(pController, pPawn);
                }
            }

            // ===== AIMBOT FOV CIRCLE (drawn here in RenderThread, not TickThread) =====
            if (CONFIG_GET(bool, g_Variables.m_AimBot.m_bEnableAimbot))
                Aimbot::DrawFOVCircle();

            // ===== RADAR =====
            if (CONFIG_GET(bool, g_Variables.m_Radar.m_bEnableRadar))
                Radar::Render(vecEntities);

            // ===== C4 TIMER =====
            if (CONFIG_GET(bool, g_Variables.m_Misc.m_bC4Timer))
            {
                try
                {
                    int iC4Count = 0;
                    int iTotalEnts = static_cast<int>(vecEntities.size());
                    for (const EntityObject_t& obj : vecEntities)
                        if (obj.m_eType == EEntityType::ENTITY_PLANTEDC4) iC4Count++;

                    char szDbg[128];
                    snprintf(szDbg, sizeof(szDbg), "[C4] ents:%d c4:%d", iTotalEnts, iC4Count);
                    Draw::AddText(ImVec2(10.f, static_cast<float>(Window::m_iHeight) - 30.f), szDbg, Color(0, 255, 255, 200), DRAW_TEXT_DROPSHADOW, Color(0, 0, 0, 200));

                    if (iC4Count > 0)
                    {
                        for (const EntityObject_t& obj : vecEntities)
                        {
                            if (obj.m_eType != EEntityType::ENTITY_PLANTEDC4) continue;
                            std::uintptr_t uC4 = reinterpret_cast<std::uintptr_t>(obj.m_pEntity);
                            if (uC4 < 0x10000) continue;

                            auto itBlow = SchemaSystem::m_mapSchemaOffsets.find(FNV1A::HashConst("C_PlantedC4->m_flC4Blow"));
                            auto itSite = SchemaSystem::m_mapSchemaOffsets.find(FNV1A::HashConst("C_PlantedC4->m_nBombSite"));

                            const char* szSite = "?";
                            if (itSite != SchemaSystem::m_mapSchemaOffsets.end() && itSite->second != 0)
                            {
                                int nSite = g_Memory.ReadMemory<int>(uC4 + itSite->second);
                                szSite = (nSite == 0) ? "A" : (nSite == 1) ? "B" : "?";
                            }

                            float flRemain = -1.f;
                            if (itBlow != SchemaSystem::m_mapSchemaOffsets.end() && itBlow->second != 0)
                            {
                                float flBlow = g_Memory.ReadMemory<float>(uC4 + itBlow->second);
                                float flCur  = g_Interfaces.m_GlobalVars.m_flCurrentTime;
                                if (std::isfinite(flBlow) && std::isfinite(flCur) && flBlow > 0.f && flCur > 0.f)
                                {
                                    flRemain = flBlow - flCur;
                                    if (flRemain < 0.f || flRemain > 60.f) flRemain = -1.f;
                                }
                            }

                            Color colTimer = (flRemain > 10.f) ? Color(255, 255, 50, 255) :
                                             (flRemain > 5.f)  ? Color(255, 150, 0, 255) :
                                             (flRemain >= 0.f) ? Color(255, 30, 30, 255) :
                                                                 Color(255, 255, 50, 255);

                            char szHud[64];
                            if (flRemain >= 0.f)
                                snprintf(szHud, sizeof(szHud), "BOMB [%s]: %.1fs", szSite, flRemain);
                            else
                                snprintf(szHud, sizeof(szHud), "BOMB [%s]", szSite);

                            Draw::AddText(ImVec2(Window::m_iWidth * 0.5f - 60.f, 60.f), szHud, colTimer, DRAW_TEXT_DROPSHADOW, Color(0, 0, 0, 200));
                            break;
                        }
                    }
                }
                catch (...) { }
            }
        }

        Draw::SwapDrawData();
    }
}

// -----------------------------------------------------------------------
void MapParserThread()
{
    SetThreadPriorityWrapper();
    while (!g_Globals.m_bIsUnloading)
    {
        if (!g_Utilities.IsInGame())
        {
            g_Utilities.Sleep(3000.f);
            continue;
        }
        std::string strMapName = g_Memory.ReadMemoryString(g_Interfaces.m_GlobalVars.m_uMapNameShort);
        if (strMapName.empty())
        {
            g_Utilities.Sleep(3000.f);
            continue;
        }
        g_MapParser.VerifyMapNameHash(strMapName);
        g_Utilities.Sleep(3000.f);
    }
}

// -----------------------------------------------------------------------
void AimbotThread()
{
    SetThreadPriorityWrapper();
    while (!g_Globals.m_bIsUnloading)
    {
        if (g_Utilities.IsInGame() && g_License.HasFeature(ETier::PRO) &&
            CONFIG_GET(bool, g_Variables.m_AimBot.m_bEnableAimbot) && !Gui::m_bOpen &&
            g_Memory.IsWindowInForeground(X("Counter-Strike 2")))
        {
            std::unique_lock lockEntityGuard(EntityList::m_mtxEntities);
            std::vector<EntityObject_t> vecEntities;
            vecEntities.assign(EntityList::m_vecEntities.begin(), EntityList::m_vecEntities.end());
            lockEntityGuard.unlock();
            lockEntityGuard.release();

            try { Aimbot::Run(vecEntities); } catch (...) { }
        }
        g_Utilities.Sleep(5.f); // 5ms = 200Hz — fast enough for smooth aim
    }
}

// -----------------------------------------------------------------------
void BhopThread()
{
    SetThreadPriorityWrapper();
    while (!g_Globals.m_bIsUnloading)
    {
        if (g_Utilities.IsInGame() && g_License.HasFeature(ETier::MID) && !Gui::m_bOpen)
        {
            try { Bhop::Run(); } catch (...) { }
        }
        g_Utilities.Sleep(0.5f); // 0.5ms = 2000Hz — maximum polling speed
    }
}

// -----------------------------------------------------------------------
void TickThread()
{
    SetThreadPriorityWrapper();
    while (!g_Globals.m_bIsUnloading)
    {
        if (!g_Utilities.IsInGame())
        {
            g_Utilities.Sleep(3000.f);
            continue;
        }

        if (!g_Memory.IsWindowInForeground(X("Counter-Strike 2")) || Gui::m_bOpen)
        {
            g_Utilities.Sleep(1000.0f);
            continue;
        }

        // Snapshot entities
        std::unique_lock lockEntityGuard(EntityList::m_mtxEntities);
        std::vector<EntityObject_t> vecEntities;
        vecEntities.assign(EntityList::m_vecEntities.begin(), EntityList::m_vecEntities.end());
        lockEntityGuard.unlock();
        lockEntityGuard.release();

        try
        {
            // ===== ESP HOTKEY TOGGLE =====
            {
                static bool bESPKeyWasDown = false;
                bool bESPKeyDown = (GetAsyncKeyState(CONFIG_GET(int, g_Variables.m_Hotkeys.m_iESPToggleKey)) & 0x8000) != 0;
                if (bESPKeyDown && !bESPKeyWasDown)
                    CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bEnableVisuals) = !CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bEnableVisuals);
                bESPKeyWasDown = bESPKeyDown;
            }

            // ===== ANTI-FLASH =====
            if (CONFIG_GET(bool, g_Variables.m_Misc.m_bAntiFlash))
            {
                C_CSPlayerPawn* pLocalPawn = g_Globals.m_LocalPlayer.m_pPlayerPawn;
                if (pLocalPawn)
                {
                    float flAlpha = pLocalPawn->m_flFlashMaxAlpha();
                    if (flAlpha > 0.f)
                    {
                        static std::uint32_t uOffset = SchemaSystem::m_mapSchemaOffsets[FNV1A::HashConst("C_CSPlayerPawnBase->m_flFlashMaxAlpha")];
                        g_Memory.WriteMemory<float>(reinterpret_cast<std::uintptr_t>(pLocalPawn) + uOffset, 0.f);
                    }
                }
            }

            // ===== TRIGGERBOT =====
            if (g_License.HasFeature(ETier::MID) && CONFIG_GET(bool, g_Variables.m_TriggerBot.m_bEnableTriggerbot))
                Triggerbot::Run(vecEntities);
        }
        catch (...) { }

        g_Utilities.Sleep(INTERVAL_PER_TICK * 1000.0f);
    }
}

// -----------------------------------------------------------------------
void UpdateThread()
{
    SetThreadPriorityWrapper();
    while (!g_Globals.m_bIsUnloading)
    {
        g_Globals.Update();
        g_Interfaces.Update();
        g_Utilities.Sleep(1);
    }
}

// -----------------------------------------------------------------------
__forceinline void CreateThreads()
{
    std::thread(&EntityThread).detach();
    std::thread(&RenderThread).detach();
    std::thread(&MapParserThread).detach();
    std::thread(&TickThread).detach();
    std::thread(&AimbotThread).detach();
    std::thread(&BhopThread).detach();
    std::thread(&UpdateThread).detach();
}

// -----------------------------------------------------------------------
bool MainLoop(LPVOID lpParameter)
{
    system(X("cls"));

#ifndef _DEBUG
    DetachConsole();
#endif

    try
    {
        g_Memory.Initialize(X("cs2.exe"));

        while (g_Memory.GetModule(NAVSYSTEM_DLL).m_uBaseAddress == 0U)
        {
            std::cout << X("Looking for navsystem.dll") << std::endl;
            g_Utilities.Sleep(500.0f);
        }

        g_License.Load();
        Config::Setup(X("default.json"));
        SchemaSystem::Setup();

        if (!Window::m_bInitialized)
            Window::Create();

        SetPriorityClass(g_Globals.m_Instance, HIGH_PRIORITY_CLASS);
        SetPriorityClass(g_Globals.m_hDll,     HIGH_PRIORITY_CLASS);
        SetPriorityClass(GetCurrentProcess(),   HIGH_PRIORITY_CLASS);

        CreateThreads();

        while (!g_Globals.m_bIsUnloading)
        {
            if (!Window::Render())
                return false;
        }
    }
    catch (const std::exception& ex)
    {
#ifdef _DEBUG
        _RPT0(_CRT_ERROR, ex.what());
#else
        FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), EXIT_FAILURE);
#endif
    }
    return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR pArgs, int iCmdShow)
{
    ConsoleAttach(X("External Base"));
    g_Globals.m_hDll = hInstance;

    if (!MainLoop(hInstance))
    {
        g_Memory.~CMemory();
        if (Window::m_bInitialized)
            Window::Destroy();
    }
    return EXIT_SUCCESS;
}
