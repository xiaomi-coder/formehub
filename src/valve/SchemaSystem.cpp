#include "../Includes.h"

bool SchemaSystem::Setup()
{
    std::uintptr_t uSchemaInterfaceAddress = g_Memory.PatternScan(SCHEMASYSTEM_DLL, X("48 89 05 ? ? ? ? 4C 8D 0D ? ? ? ? 33 C0 48 C7 05 ? ? ? ? ? ? ? ? 89 05"), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7);
    std::uintptr_t uSchemaSystemScopeArrayPtr = 0U;
    if (!g_Memory.ReadMemoryRaw(uSchemaInterfaceAddress + CS_OFFSETOF(CSchemaSystem, m_pScopeArray), &uSchemaSystemScopeArrayPtr, sizeof(std::uintptr_t)))
    {
        std::cout << X("Failed to read scope array ptr") << std::endl;
        return false;
    }
  
    int nScopeSize = g_Memory.ReadMemory<int>(uSchemaInterfaceAddress + CS_OFFSETOF(CSchemaSystem, m_nScopeSize));
    void** ppScopeArray = new void* [nScopeSize];

    if (!g_Memory.ReadMemoryRaw(uSchemaSystemScopeArrayPtr, ppScopeArray, (nScopeSize * sizeof(void*))))
    {
        std::cout << X("Failed to read scope array") << std::endl;
        return false;
    }

    for (std::uint16_t i = 0U; i <= nScopeSize; ++i)
    {
        CSchemaSystemTypeScope schemaScope{};
        if (!g_Memory.ReadMemoryRaw(ppScopeArray[i], &schemaScope, sizeof(CSchemaSystemTypeScope)) || !schemaScope.m_pDeclaredClasses)
            continue;

        CSchemaDeclaredClassEntry* pDeclaredClassEntries = new CSchemaDeclaredClassEntry[schemaScope.m_uNumDeclaredClasses + 1U];
        if (!g_Memory.ReadMemoryRaw(schemaScope.m_pDeclaredClasses, pDeclaredClassEntries, (schemaScope.m_uNumDeclaredClasses + 1U) * sizeof(CSchemaDeclaredClassEntry)))
            continue;
     
        if (strcmp(schemaScope.m_szName, X("client.dll")) == 0)
        {     
            for (std::uint16_t j = 0U; j < schemaScope.m_uNumDeclaredClasses; ++j)
            {
                CSchemaDeclaredClass declaredClass{ };
                if (!g_Memory.ReadMemoryRaw(pDeclaredClassEntries[j].m_pDeclaredClass, &declaredClass, sizeof(CSchemaDeclaredClass)))
                    continue;

                CSchemaClass schemaClass{ };
                if (!g_Memory.ReadMemoryRaw(declaredClass.m_Class, &schemaClass, sizeof(CSchemaClass)))
                    continue;

                char szClassName[128]{};
                if (!g_Memory.ReadMemoryRaw((void*)(declaredClass.m_szName), szClassName, sizeof(szClassName)))
                    continue;

                std::uintptr_t uClassFieldsPtr = reinterpret_cast<uintptr_t>(schemaClass.m_pFields);
                if (uClassFieldsPtr)
                {
                    for (std::uint16_t k = 0; k < schemaClass.m_uNumFields; ++k)
                    {
                        CSchemaField schemaField = g_Memory.ReadMemory<CSchemaField>(uClassFieldsPtr + (sizeof(CSchemaField) * k));
                        if (!schemaField.m_pType)
                            continue;

                        char szFieldName[128] = { 0 };
                        if (!g_Memory.ReadMemoryRaw((void*)(schemaField.m_szName), szFieldName, sizeof(szFieldName)))
                            continue;

                        const std::string strSchemaField = std::vformat(X("{}->{}"), std::make_format_args(szClassName, szFieldName));
                        m_mapSchemaOffsets[FNV1A::Hash(strSchemaField.c_str())] = schemaField.m_uOffset;
                    }
                }
            }
        }    
    }

    delete[] ppScopeArray;

	// ---------------------------------------------------------------
	// HARDCODED FALLBACK OFFSETS (from cs2-dumper, updated 2026-04)
	// If schema system failed to populate these, use known-good values
	// ---------------------------------------------------------------
	auto SetIfZero = [](FNV1A_t hash, std::uint32_t fallback) {
		if (m_mapSchemaOffsets.find(hash) == m_mapSchemaOffsets.end() || m_mapSchemaOffsets[hash] == 0)
			m_mapSchemaOffsets[hash] = fallback;
	};

	// C_BaseEntity
	SetIfZero(FNV1A::HashConst("C_BaseEntity->m_pGameSceneNode"), 816);
	SetIfZero(FNV1A::HashConst("C_BaseEntity->m_iHealth"), 844);
	SetIfZero(FNV1A::HashConst("C_BaseEntity->m_iMaxHealth"), 840);
	SetIfZero(FNV1A::HashConst("C_BaseEntity->m_iTeamNum"), 1003);
	SetIfZero(FNV1A::HashConst("C_BaseEntity->m_lifeState"), 852);
	SetIfZero(FNV1A::HashConst("C_BaseEntity->m_pCollision"), 832);
	SetIfZero(FNV1A::HashConst("C_BaseEntity->m_fFlags"), 1016);
	SetIfZero(FNV1A::HashConst("C_BaseEntity->m_hOwnerEntity"), 1312);
	SetIfZero(FNV1A::HashConst("C_BaseEntity->m_vecVelocity"), 1032);
	SetIfZero(FNV1A::HashConst("C_BaseEntity->m_vecAbsVelocity"), 1048);
	SetIfZero(FNV1A::HashConst("C_BaseEntity->m_nSubclassID"), 896);

	// CCSPlayerController
	SetIfZero(FNV1A::HashConst("CCSPlayerController->m_sSanitizedPlayerName"), 2136);

	// C_PlantedC4
	SetIfZero(FNV1A::HashConst("C_PlantedC4->m_flC4Blow"), 4496);
	SetIfZero(FNV1A::HashConst("C_PlantedC4->m_bBombDefused"), 4532);

	// CGameSceneNode
	SetIfZero(FNV1A::HashConst("CGameSceneNode->m_vecAbsOrigin"), 200);
	SetIfZero(FNV1A::HashConst("CGameSceneNode->m_bDormant"), 259);
	SetIfZero(FNV1A::HashConst("CGameSceneNode->m_nodeToWorld"), 16);

	// CCollisionProperty
	SetIfZero(FNV1A::HashConst("CCollisionProperty->m_vecMins"), 64);
	SetIfZero(FNV1A::HashConst("CCollisionProperty->m_vecMaxs"), 76);

	// CBasePlayerController
	SetIfZero(FNV1A::HashConst("CBasePlayerController->m_hPawn"), 1724);
	SetIfZero(FNV1A::HashConst("CBasePlayerController->m_iszPlayerName"), 1776);

	// C_BaseModelEntity
	SetIfZero(FNV1A::HashConst("C_BaseModelEntity->m_vecViewOffset"), 3696);
	SetIfZero(FNV1A::HashConst("C_BaseModelEntity->m_Glow"), 3544);

	// C_EconEntity
	SetIfZero(FNV1A::HashConst("C_EconEntity->m_AttributeManager"), 4480);
	SetIfZero(FNV1A::HashConst("C_AttributeContainer->m_Item"), 80);
	SetIfZero(FNV1A::HashConst("C_EconItemView->m_iItemDefinitionIndex"), 442);

	// CGlowProperty
	SetIfZero(FNV1A::HashConst("CGlowProperty->m_iGlowType"), 48);
	SetIfZero(FNV1A::HashConst("CGlowProperty->m_glowColorOverride"), 64);
	SetIfZero(FNV1A::HashConst("CGlowProperty->m_bGlowing"), 81);
	SetIfZero(FNV1A::HashConst("CGlowProperty->m_nGlowRange"), 56);
	SetIfZero(FNV1A::HashConst("CGlowProperty->m_nGlowRangeMin"), 60);

	// C_BasePlayerPawn
	SetIfZero(FNV1A::HashConst("C_BasePlayerPawn->m_pWeaponServices"), 4576);

	// CPlayer_WeaponServices
	SetIfZero(FNV1A::HashConst("CPlayer_WeaponServices->m_hActiveWeapon"), 96);
	SetIfZero(FNV1A::HashConst("CPlayer_WeaponServices->m_hMyWeapons"), 72);

	// CBasePlayerController - CRITICAL for ESP
	SetIfZero(FNV1A::HashConst("CBasePlayerController->m_bIsLocalPlayerController"), 1920);

	// CCSPlayerController
	SetIfZero(FNV1A::HashConst("CCSPlayerController->m_hPlayerPawn"), 2060);
	SetIfZero(FNV1A::HashConst("CCSPlayerController->m_sSanitizedPlayerName"), 1848);
	SetIfZero(FNV1A::HashConst("CCSPlayerController->m_iCompTeammateColor"), 1916);

	// C_CSPlayerPawnBase / C_CSPlayerPawn
	SetIfZero(FNV1A::HashConst("C_CSPlayerPawnBase->m_flFlashMaxAlpha"), 5116);
	SetIfZero(FNV1A::HashConst("C_CSPlayerPawnBase->m_flFlashDuration"), 5120);
	SetIfZero(FNV1A::HashConst("C_CSPlayerPawnBase->m_entitySpottedState"), 4464);
	SetIfZero(FNV1A::HashConst("C_CSPlayerPawn->m_entitySpottedState"), 4464);

	// EntitySpottedState_t
	SetIfZero(FNV1A::HashConst("EntitySpottedState_t->m_bSpotted"), 8);
	SetIfZero(FNV1A::HashConst("EntitySpottedState_t->m_bSpottedByMask"), 12);

	std::cout << "  [+] Schema offsets: " << m_mapSchemaOffsets.size() << " entries (with fallbacks)" << std::endl;

	return m_mapSchemaOffsets.size() > 0;
}