#include "../Includes.h"
#include "SkinTable.h"
#include "ApplySkins.h"

// -----------------------------------------------------------------------
// Internal skin applier.
//
// Uses the exact write sequence that was validated live from the external
// build (values verified to persist in memory, see the debug panel work in
// src/features/skinchanger/SkinChanger.cpp), but runs in-process so the
// writes land on the same tick the game composes the weapon.
// -----------------------------------------------------------------------

namespace
{
    // --- resolved schema offsets ---
    std::uintptr_t g_uAttrMgr          = 0;
    std::uintptr_t g_uItem             = 0;
    std::uintptr_t g_uItemDefIdx       = 0;
    std::uintptr_t g_uItemIDHigh       = 0;
    std::uintptr_t g_uItemIDLow        = 0;
    std::uintptr_t g_uItemID           = 0;
    std::uintptr_t g_uAccountID        = 0;
    std::uintptr_t g_uInitialized      = 0;
    std::uintptr_t g_uFallbackPaint    = 0;
    std::uintptr_t g_uFallbackSeed     = 0;
    std::uintptr_t g_uFallbackWear     = 0;
    std::uintptr_t g_uFallbackStatTrak = 0;

    bool g_bResolved = false;

    std::uintptr_t Resolve(const char* szName)
    {
        auto it = SchemaSystem::m_mapSchemaOffsets.find(FNV1A::Hash(szName));
        if (it != SchemaSystem::m_mapSchemaOffsets.end() && it->second != 0)
            return it->second;

        return 0;
    }

    bool IsKnifeDefIndex(std::uint16_t idx)
    {
        switch (idx)
        {
        case WEAPON_KNIFE_GG:
        case WEAPON_KNIFE_CT:
        case WEAPON_KNIFE_T:
        case WEAPON_BAYONET:
        case WEAPON_CLASSIC_KNIFE:
        case WEAPON_FLIP_KNIFE:
        case WEAPON_GUT_KNIFE:
        case WEAPON_KARAMBIT:
        case WEAPON_M9_BAYONET:
        case WEAPON_HUNTSMAN_KNIFE:
        case WEAPON_FALCHION_KNIFE:
        case WEAPON_BOWIE_KNIFE:
        case WEAPON_BUTTERFLY_KNIFE:
        case WEAPON_SHADOW_DAGGERS:
        case WEAPON_PARACORD_KNIFE:
        case WEAPON_SURVIVAL_KNIFE:
        case WEAPON_URSUS_KNIFE:
        case WEAPON_NAVAJA_KNIFE:
        case WEAPON_NOMAD_KNIFE:
        case WEAPON_STILETTO_KNIFE:
        case WEAPON_TALON_KNIFE:
        case WEAPON_SKELETON_KNIFE:
        case WEAPON_KUKRI_KNIFE:
            return true;
        default:
            return false;
        }
    }

    // Log a weapon only when what we apply to it actually changes, so the
    // console stays readable instead of scrolling every tick.
    std::map<std::uintptr_t, int> g_mapLogged;
}

bool InternalSkins::ResolveOffsets()
{
    if (g_bResolved)
        return true;

    g_uAttrMgr          = Resolve("C_EconEntity->m_AttributeManager");
    g_uItem             = Resolve("C_AttributeContainer->m_Item");
    g_uItemDefIdx       = Resolve("C_EconItemView->m_iItemDefinitionIndex");
    g_uItemIDHigh       = Resolve("C_EconItemView->m_iItemIDHigh");
    g_uItemIDLow        = Resolve("C_EconItemView->m_iItemIDLow");
    g_uItemID           = Resolve("C_EconItemView->m_iItemID");
    g_uAccountID        = Resolve("C_EconItemView->m_iAccountID");
    g_uInitialized      = Resolve("C_EconItemView->m_bInitialized");
    g_uFallbackPaint    = Resolve("C_EconEntity->m_nFallbackPaintKit");
    g_uFallbackSeed     = Resolve("C_EconEntity->m_nFallbackSeed");
    g_uFallbackWear     = Resolve("C_EconEntity->m_flFallbackWear");
    g_uFallbackStatTrak = Resolve("C_EconEntity->m_nFallbackStatTrak");

    if (g_uAttrMgr == 0 || g_uItem == 0 || g_uItemDefIdx == 0 || g_uFallbackPaint == 0)
    {
        std::cout << "  [SKIN] Kerakli schema offsetlar topilmadi!" << std::endl;
        std::cout << "         AttrMgr=" << g_uAttrMgr
                  << " Item="     << g_uItem
                  << " DefIdx="   << g_uItemDefIdx
                  << " Paint="    << g_uFallbackPaint << std::endl;
        return false;
    }

    g_bResolved = true;

    std::cout << "  [SKIN] Offsetlar tayyor:" << std::endl;
    std::cout << "         AttrMgr="     << g_uAttrMgr
              << " Item="                << g_uItem
              << " DefIdx="              << g_uItemDefIdx    << std::endl;
    std::cout << "         Paint="       << g_uFallbackPaint
              << " Wear="                << g_uFallbackWear
              << " Seed="                << g_uFallbackSeed
              << " StatTrak="            << g_uFallbackStatTrak << std::endl;
    std::cout << "         IDHigh="      << g_uItemIDHigh
              << " IDLow="               << g_uItemIDLow
              << " ItemID="              << g_uItemID
              << " AccountID="           << g_uAccountID
              << " Initialized="         << g_uInitialized   << std::endl;

    return true;
}

void InternalSkins::Run()
{
    if (!g_bResolved && !ResolveOffsets())
        return;

    C_CSPlayerPawn* pLocalPawn = g_Globals.m_LocalPlayer.m_pPlayerPawn;
    if (!pLocalPawn || reinterpret_cast<std::uintptr_t>(pLocalPawn) < 0x10000)
        return;

    if (!pLocalPawn->IsAlive())
        return;

    CCSPlayer_WeaponServices* pWeaponServices = pLocalPawn->m_pWeaponServices();
    if (!pWeaponServices || reinterpret_cast<std::uintptr_t>(pWeaponServices) < 0x1000)
        return;

    C_NetworkUtlVectorBaseSimple hWeapons = pWeaponServices->m_hMyWeapons();
    if (hWeapons.m_nSize <= 0 || hWeapons.m_nSize > 16 || hWeapons.m_pData < 0x1000)
        return;

    for (int i = 0; i < hWeapons.m_nSize; ++i)
    {
        CHandle<C_BasePlayerWeapon> hWeapon =
            g_Memory.ReadMemory<CHandle<C_BasePlayerWeapon>>(hWeapons.m_pData + (i * 0x4));

        if (!hWeapon.IsValid())
            continue;

        C_BasePlayerWeapon* pWeapon = hWeapon.Get();
        if (!pWeapon || reinterpret_cast<std::uintptr_t>(pWeapon) < 0x1000)
            continue;

        const std::uintptr_t uWeaponAddr = reinterpret_cast<std::uintptr_t>(pWeapon);
        const std::uintptr_t uEconBase   = uWeaponAddr + g_uAttrMgr + g_uItem;

        const std::uint16_t nDefIndex = g_Memory.ReadMemory<std::uint16_t>(uEconBase + g_uItemDefIdx);
        if (nDefIndex == 0)
            continue;

        // Pick the skin: any knife shares one entry, everything else is looked up.
        const char* szSkinName = nullptr;
        int nPaintKit = 0;

        if (IsKnifeDefIndex(nDefIndex))
        {
            nPaintKit  = SkinTable::g_nKnifePaintKit;
            szSkinName = SkinTable::g_szKnifeName;
        }
        else
        {
            nPaintKit = SkinTable::Lookup(nDefIndex, &szSkinName);
        }

        if (nPaintKit == 0)
            continue;

        // 1. Force the item out of "real inventory item" mode so the game
        //    falls back to the m_nFallback* fields below.
        g_Memory.WriteMemory<int>(uEconBase + g_uItemIDHigh, -1);

        if (g_uItemIDLow > 0)
            g_Memory.WriteMemory<int>(uEconBase + g_uItemIDLow, -1);

        if (g_uItemID > 0)
            g_Memory.WriteMemory<std::int64_t>(uEconBase + g_uItemID, -1);

        if (g_uAccountID > 0)
            g_Memory.WriteMemory<int>(uEconBase + g_uAccountID, 0);

        // 2. Mark the econ view as fully built, otherwise the game keeps
        //    rendering the material it already composed for the real item.
        if (g_uInitialized > 0)
            g_Memory.WriteMemory<bool>(uEconBase + g_uInitialized, true);

        // 3. The actual skin.
        g_Memory.WriteMemory<int>  (uWeaponAddr + g_uFallbackPaint, nPaintKit);

        if (g_uFallbackWear > 0)
            g_Memory.WriteMemory<float>(uWeaponAddr + g_uFallbackWear, SkinTable::g_flWear);

        if (g_uFallbackSeed > 0)
            g_Memory.WriteMemory<int>(uWeaponAddr + g_uFallbackSeed, SkinTable::g_nSeed);

        if (SkinTable::g_nStatTrak >= 0 && g_uFallbackStatTrak > 0)
            g_Memory.WriteMemory<int>(uWeaponAddr + g_uFallbackStatTrak, SkinTable::g_nStatTrak);

        // Diagnostics: report each weapon once per paint kit change.
        auto itLogged = g_mapLogged.find(uWeaponAddr);
        if (itLogged == g_mapLogged.end() || itLogged->second != nPaintKit)
        {
            g_mapLogged[uWeaponAddr] = nPaintKit;
            std::cout << "  [SKIN] def=" << nDefIndex
                      << " paint="       << nPaintKit
                      << "  "            << (szSkinName ? szSkinName : "?")
                      << std::endl;
        }
    }
}
