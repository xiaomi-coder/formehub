#include "../Includes.h"
#include "SkinTable.h"
#include "ApplySkins.h"
#include "AutoGlow.h"
#include "InternalMain.h"

// -----------------------------------------------------------------------
// Worker thread running inside cs2.exe.
//
// No rendering, no menu, no hooks — it only walks the entity list and
// writes skin + glow fields, exactly like the external build does, except
// in-process.
// -----------------------------------------------------------------------

namespace
{
    FILE* g_pConsoleOut = nullptr;
    FILE* g_pLogFile = nullptr;

    void LogDebug(const char* szMsg)
    {
        if (!g_pLogFile)
        {
            char szPath[MAX_PATH];
            GetTempPathA(sizeof(szPath), szPath);
            strcat_s(szPath, sizeof(szPath), "SkinCore_debug.log");
            fopen_s(&g_pLogFile, szPath, "a");
        }
        if (g_pLogFile)
        {
            fprintf(g_pLogFile, "%s", szMsg);
            fflush(g_pLogFile);
        }
    }

    void OpenConsole()
    {
        LogDebug("[OpenConsole] Attempting AllocConsole\n");
        if (!AllocConsole())
        {
            LogDebug("[OpenConsole] AllocConsole failed\n");
            return;
        }

        LogDebug("[OpenConsole] AllocConsole succeeded\n");
        freopen_s(&g_pConsoleOut, "CONOUT$", "w", stdout);
        SetConsoleTitleA("SHIFTHUB SkinCore");

        std::cout << "  ===============================" << std::endl;
        std::cout << "   SHIFTHUB SkinCore (internal)"   << std::endl;
        std::cout << "  ===============================" << std::endl;
    }

    void CloseConsole()
    {
        if (g_pConsoleOut)
        {
            fclose(g_pConsoleOut);
            g_pConsoleOut = nullptr;
        }

        FreeConsole();

        if (g_pLogFile)
        {
            fclose(g_pLogFile);
            g_pLogFile = nullptr;
        }
    }
}

void InternalCore::Run(HMODULE hSelf)
{
    try
    {
        // Try file logging first, before console
        LogDebug("[InternalCore::Run] Thread started\n");

        // This thread is started from DllMain, so LoadLibrary still holds the
        // loader lock for a moment. AllocConsole and friends load further DLLs,
        // which would block on that lock — give the loader time to finish first.
        LogDebug("[InternalCore::Run] Sleeping 500ms\n");
        Sleep(500);
        LogDebug("[InternalCore::Run] Sleep done, opening console\n");

        OpenConsole();
        LogDebug("[InternalCore::Run] Console opened\n");
        std::cout << "  [InternalCore] Started\n";

        // Attach the shared memory layer to our OWN process, so every existing
        // Read/Write/Schema/Entity helper works unchanged in-process.
        std::cout << "  [InternalCore] Initializing memory...\n";
        g_Memory.InitializeInternal();
        std::cout << "  [+] Jarayonga ulandi (PID " << GetCurrentProcessId() << ")" << std::endl;

        // The injector may run before the game modules are mapped.
        std::cout << "  [InternalCore] Waiting for client.dll...\n"; std::cout.flush();
        int iAttempts = 0;
        while (g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress == 0U)
        {
            Sleep(500);
            if (++iAttempts > 60) // 30 seconds
            {
                std::cout << "  [X] client.dll topilmadi — to'xtatilmoqda." << std::endl;
                CloseConsole();
                FreeLibraryAndExitThread(hSelf, 0);
            }
        }

        std::cout << "  [+] client.dll: 0x" << std::hex
                  << g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress << std::dec << std::endl;
        std::cout.flush();

        // Live schema walk; falls back to the a2x-validated table on failure.
        std::cout << "  [~] Schema o'qilmoqda...\n"; std::cout.flush();
        SchemaSystem::Setup();
        std::cout << "  [+] Schema setup done\n"; std::cout.flush();

        std::cout << "  [~] Resolving skin offsets...\n"; std::cout.flush();
        if (!InternalSkins::ResolveOffsets())
            std::cout << "  [!] Skin offsetlari tayyor emas — keyingi tick'da qayta urinamiz." << std::endl;
        std::cout << "  [+] Offsets resolved\n"; std::cout.flush();

        std::cout << "  [+] Ishga tushdi. Skin + glow faol." << std::endl;
        std::cout << "  ---------------------------------" << std::endl;
        std::cout.flush();

        int iTickCount = 0;
        while (!g_Globals.m_bIsUnloading)
        {
            try
            {
                if (iTickCount % 100 == 0)
                    std::cout << "  [tick " << iTickCount << "]\n"; std::cout.flush();

                std::cout << "    [Update]\n"; std::cout.flush();
                g_Globals.Update();

                std::cout << "    [EntityList]\n"; std::cout.flush();
                EntityList::UpdateEntities();

                std::cout << "    [Glow]\n"; std::cout.flush();
                std::vector<EntityObject_t> vecEntities;
                vecEntities.assign(EntityList::m_vecEntities.begin(), EntityList::m_vecEntities.end());
                InternalGlow::Run(vecEntities);

                std::cout << "    [Skins]\n"; std::cout.flush();
                InternalSkins::Run();

                if (iTickCount % 100 == 0)
                    std::cout.flush();
                iTickCount++;
            }
            catch (const std::exception& e)
            {
                std::cout << "  [Exception in tick] " << e.what() << "\n"; std::cout.flush();
            }
            catch (...)
            {
                std::cout << "  [Unknown exception in tick]\n"; std::cout.flush();
            }

            Sleep(50);
        }

        CloseConsole();
    }
    catch (const std::exception& e)
    {
        OutputDebugStringA("[InternalCore] Exception caught: ");
        OutputDebugStringA(e.what());
        OutputDebugStringA("\n");
        std::cout << "  [InternalCore] Exception: " << e.what() << "\n";
        CloseConsole();
    }
    catch (...)
    {
        OutputDebugStringA("[InternalCore] Unknown exception caught\n");
        std::cout << "  [InternalCore] Unknown exception\n";
        CloseConsole();
    }

    OutputDebugStringA("[InternalCore::Run] Exiting thread\n");
    FreeLibraryAndExitThread(hSelf, 0);
}
