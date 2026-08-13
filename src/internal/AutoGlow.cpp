#include "../Includes.h"
#include "AutoGlow.h"

// -----------------------------------------------------------------------
// Enemy glow, hardcoded.
//
// Same engine-native glow approach as src/features/esp/PlayerGlow.cpp
// (writes CGlowProperty on C_BaseModelEntity->m_Glow), but with fixed
// settings instead of CONFIG_GET, since this tool has no settings UI.
// -----------------------------------------------------------------------

namespace
{
    constexpr int GLOW_TYPE  = 3;      // outer glow, visible through walls
    constexpr int GLOW_RANGE = 16384;  // max range

    // Enemy colour (RGBA)
    constexpr int GLOW_R = 255;
    constexpr int GLOW_G = 40;
    constexpr int GLOW_B = 40;
    constexpr int GLOW_A = 255;

    std::uintptr_t g_uGlow         = 0;
    std::uintptr_t g_uGlowType     = 0;
    std::uintptr_t g_uGlowColor    = 0;
    std::uintptr_t g_uGlowing      = 0;
    std::uintptr_t g_uGlowRange    = 0;
    std::uintptr_t g_uGlowRangeMin = 0;

    bool g_bResolved = false;
    bool g_bTried    = false;

    std::uintptr_t Resolve(const char* szName)
    {
        auto it = SchemaSystem::m_mapSchemaOffsets.find(FNV1A::Hash(szName));
        if (it != SchemaSystem::m_mapSchemaOffsets.end() && it->second != 0)
            return it->second;

        return 0;
    }

    void ResolveOffsets()
    {
        if (g_bTried)
            return;

        g_bTried = true;

        g_uGlow         = Resolve("C_BaseModelEntity->m_Glow");
        g_uGlowType     = Resolve("CGlowProperty->m_iGlowType");
        g_uGlowColor    = Resolve("CGlowProperty->m_glowColorOverride");
        g_uGlowing      = Resolve("CGlowProperty->m_bGlowing");
        g_uGlowRange    = Resolve("CGlowProperty->m_nGlowRange");
        g_uGlowRangeMin = Resolve("CGlowProperty->m_nGlowRangeMin");

        g_bResolved = (g_uGlow > 0 && g_uGlowing > 0);

        std::cout << "  [GLOW] Glow=" << g_uGlow
                  << " Type="         << g_uGlowType
                  << " Color="        << g_uGlowColor
                  << " Glowing="      << g_uGlowing
                  << " Range="        << g_uGlowRange
                  << "  ->  "         << (g_bResolved ? "OK" : "TOPILMADI") << std::endl;
    }
}

void InternalGlow::Run(const std::vector<EntityObject_t>& vecEntities)
{
    ResolveOffsets();
    if (!g_bResolved)
        return;

    C_CSPlayerPawn* pLocalPawn = g_Globals.m_LocalPlayer.m_pPlayerPawn;
    if (!pLocalPawn || reinterpret_cast<std::uintptr_t>(pLocalPawn) < 0x10000)
        return;

    if (!pLocalPawn->IsAlive())
        return;

    const std::uint8_t nLocalTeam = pLocalPawn->m_iTeamNum();

    for (const EntityObject_t& obj : vecEntities)
    {
        if (obj.m_eType != EEntityType::ENTITY_PLAYER)
            continue;

        CCSPlayerController* pController = reinterpret_cast<CCSPlayerController*>(obj.m_pEntity);
        if (!pController)
            continue;

        if (pController->m_bIsLocalPlayerController())
            continue;

        C_CSPlayerPawn* pPawn = reinterpret_cast<C_CSPlayerPawn*>(pController->m_hPawn().Get());
        if (!pPawn || reinterpret_cast<std::uintptr_t>(pPawn) < 0x10000)
            continue;

        if (!pPawn->IsAlive())
            continue;

        // Enemies only.
        if (pPawn->m_iTeamNum() == nLocalTeam)
            continue;

        const std::uintptr_t uGlowBase = reinterpret_cast<std::uintptr_t>(pPawn) + g_uGlow;

        if (g_uGlowType > 0)
            g_Memory.WriteMemory<int>(uGlowBase + g_uGlowType, GLOW_TYPE);

        if (g_uGlowColor > 0)
        {
            const int iColor = GLOW_R | (GLOW_G << 8) | (GLOW_B << 16) | (GLOW_A << 24);
            g_Memory.WriteMemory<int>(uGlowBase + g_uGlowColor, iColor);
        }

        g_Memory.WriteMemory<bool>(uGlowBase + g_uGlowing, true);

        if (g_uGlowRange > 0)
            g_Memory.WriteMemory<int>(uGlowBase + g_uGlowRange, GLOW_RANGE);

        if (g_uGlowRangeMin > 0)
            g_Memory.WriteMemory<int>(uGlowBase + g_uGlowRangeMin, 0);
    }
}
