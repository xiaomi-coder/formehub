#pragma once
#include <cstddef>
#include <cstdint>

// ============================================================================
// OFFSETS — CS2 (a2x/cs2-dumper, 2026-10-15 13:10 UTC)
//
// UPDATED 2026-10-16. Regenerated against the a2x dump published 2026-10-15.
// The global offsets all moved again (see block below); among the schema
// fields only the C_CSPlayerPawn block shifted (m_bIsWalking .. m_ArmorValue,
// -8/-4). Everything else matched the previous dump.
//
// Source of truth is the cs2-dumper output vendored next to this file:
//   - globals -> <repo>/src/internal/EpsteinModule/sdk/offsets.hpp
//   - fields  -> <repo>/src/internal/EpsteinModule/sdk/client_dll.json
// Both carry the same build stamp as the header above. Decimals in the
// trailing comments; hex in the values.
//
// Markers:
//   [OK]      value matches the schema dump for the named class
//   [DERIVED] not in the dump; shifted by the delta measured on neighbouring
//             fields of the same struct (see note at each block)
//   [STALE]   not a networked field, so absent from the schema dump and
//             unverifiable — do not build new logic on these
// ============================================================================

namespace Offsets
{
    // === Global Offsets — offsets.json "client.dll" ============ [OK] =======
    constexpr std::ptrdiff_t dwEntityList                          = 0x271A908;    // 41003272
    constexpr std::ptrdiff_t dwLocalPlayerController               = 0x253CB78;    // 39046008
    constexpr std::ptrdiff_t dwLocalPlayerPawn                     = 0x2565718;    // 39212824
    constexpr std::ptrdiff_t dwPlantedC4                           = 0x24CB3C0;    // 38581184
    constexpr std::ptrdiff_t dwViewAngles                          = 0x257BCC8;    // 39304392
    constexpr std::ptrdiff_t dwCSGOInput                           = 0x257B640;    // 39302720
    constexpr std::ptrdiff_t dwGameRules                           = 0x25651D0;    // 39211472
    constexpr std::ptrdiff_t dwGlobalVars                          = 0x2230090;    // 35848336
    constexpr std::ptrdiff_t dwGlowManager                         = 0x2561EF0;    // 39198448
    constexpr std::ptrdiff_t dwPrediction                          = 0x2565620;    // 39212576
    constexpr std::ptrdiff_t dwGameEntitySystem_highestEntityIndex = 0x2120;    // 8480

    // === C_BaseEntity ========================================== [OK] =======
    // NB: every field here moved 8 bytes vs the old dump. skin_changer.h used
    // to hardcode 0x354/0x35C for health/lifeState — those were the old values.
    constexpr std::ptrdiff_t m_pGameSceneNode   = 0x330; // 816   CGameSceneNode*
    constexpr std::ptrdiff_t m_pCollision       = 0x340; // 832
    constexpr std::ptrdiff_t m_iMaxHealth       = 0x348; // 840
    constexpr std::ptrdiff_t m_iHealth          = 0x34C; // 844   int32
    constexpr std::ptrdiff_t m_lifeState        = 0x354; // 852   uint8 (0 = alive)
    constexpr std::ptrdiff_t m_nSubclassID      = 0x380; // 896
    constexpr std::ptrdiff_t m_iTeamNum         = 0x3E7; // 999   uint8
    constexpr std::ptrdiff_t m_fFlags           = 0x3F4; // 1012
    constexpr std::ptrdiff_t m_vecAbsVelocity   = 0x3F8; // 1016
    constexpr std::ptrdiff_t m_vecVelocity      = 0x430; // 1072
    constexpr std::ptrdiff_t m_hOwnerEntity     = 0x520; // 1312  CHandle<C_BaseEntity>

    // === CGameSceneNode ======================================== [OK] =======
    constexpr std::ptrdiff_t m_nodeToWorld   = 0x10;  // 16
    constexpr std::ptrdiff_t m_vecAbsOrigin  = 0xC8;  // 200
    constexpr std::ptrdiff_t m_bDormant      = 0x103; // 259

    // === CCollisionProperty ==================================== [OK] =======
    constexpr std::ptrdiff_t m_vecMins = 0x40; // 64
    constexpr std::ptrdiff_t m_vecMaxs = 0x4C; // 76

    // === C_BasePlayerPawn — services block ===================== [OK] =======
    // Whole block shifted -0x1D0 from the old dump.
    constexpr std::ptrdiff_t m_pWeaponServices   = 0x12F0; // 4848
    constexpr std::ptrdiff_t m_pObserverServices = 0x1308; // 4872
    constexpr std::ptrdiff_t m_pCameraServices   = 0x1328; // 4904  [DERIVED] -0x1D0
    constexpr std::ptrdiff_t m_pMovementServices = 0x1330; // 4912
    constexpr std::ptrdiff_t m_pViewModelServices = 0x1250; // 4688

    // === CPlayer_WeaponServices ================================ [OK] =======
    constexpr std::ptrdiff_t m_hMyWeapons    = 0x48; // 72  C_NetworkUtlVectorBase<CHandle>
    constexpr std::ptrdiff_t m_hActiveWeapon = 0x60; // 96  CHandle<C_BasePlayerWeapon>

    // === CPlayer_ObserverServices ============================== [OK] =======
    constexpr std::ptrdiff_t m_iObserverMode   = 0x48; // 72
    constexpr std::ptrdiff_t m_hObserverTarget = 0x4C; // 76

    // === CBasePlayerController ================================= [OK] =======
    constexpr std::ptrdiff_t m_hPawn                    = 0x6BC; // 1724
    constexpr std::ptrdiff_t m_iszPlayerName            = 0x6FC; // 1788
    constexpr std::ptrdiff_t m_bIsLocalPlayerController = 0x790; // 1936

    // === CCSPlayerController =================================== [OK] =======
    constexpr std::ptrdiff_t m_iCompTeammateColor   = 0x858; // 2136
    constexpr std::ptrdiff_t m_sSanitizedPlayerName = 0x878; // 2168  CUtlString
    constexpr std::ptrdiff_t m_hPlayerPawn          = 0x92C; // 2348  CHandle<C_CSPlayerPawn>
    constexpr std::ptrdiff_t m_hObserverPawn        = 0x930; // 2352
    constexpr std::ptrdiff_t m_bPawnIsAlive         = 0x934; // 2356

    // === C_BaseModelEntity ===================================== [OK] =======
    constexpr std::ptrdiff_t m_Glow          = 0xDE8; // 3560  CGlowProperty
    constexpr std::ptrdiff_t m_vecViewOffset = 0xF60; // 3936

    // === CGlowProperty ========================================= [OK] =======
    constexpr std::ptrdiff_t m_iGlowType         = 0x30; // 48
    constexpr std::ptrdiff_t m_nGlowRange        = 0x38; // 56
    constexpr std::ptrdiff_t m_nGlowRangeMin     = 0x3C; // 60
    constexpr std::ptrdiff_t m_glowColorOverride = 0x40; // 64
    constexpr std::ptrdiff_t m_bGlowing          = 0x51; // 81

    // === C_CSPlayerPawnBase / C_CSPlayerPawn =================== [OK] =======
    constexpr std::ptrdiff_t m_flFlashMaxAlpha   = 0x150C; // 5388
    constexpr std::ptrdiff_t m_flFlashDuration   = 0x1510; // 5392
    constexpr std::ptrdiff_t m_bIsWalking        = 0x1E78; // 7800
    constexpr std::ptrdiff_t m_entitySpottedState = 0x1E80; // 7808
    constexpr std::ptrdiff_t m_bIsScoped         = 0x1E98; // 7832
    constexpr std::ptrdiff_t m_bIsDefusing       = 0x1E9A; // 7834
    constexpr std::ptrdiff_t m_iShotsFired       = 0x1EB0; // 7856
    constexpr std::ptrdiff_t m_ArmorValue        = 0x1EC8; // 7880

    // === EntitySpottedState_t ================================== [OK] =======
    constexpr std::ptrdiff_t m_bSpotted       = 0x8;  // 8
    constexpr std::ptrdiff_t m_bSpottedByMask = 0xC;  // 12

    // === SKIN CHANGER — C_EconEntity =========================== [OK] =======
    // Whole block shifted -0x1D0 from the old dump. These four fallback fields
    // are what actually decide which skin the weapon renders.
    constexpr std::ptrdiff_t m_AttributeManager      = 0x1290; // 4752  C_AttributeContainer
    constexpr std::ptrdiff_t m_OriginalOwnerXuidLow  = 0x18A0; // 6304
    constexpr std::ptrdiff_t m_OriginalOwnerXuidHigh = 0x18A4; // 6308
    constexpr std::ptrdiff_t m_nFallbackPaintKit     = 0x18A8; // 6312  int32
    constexpr std::ptrdiff_t m_nFallbackSeed         = 0x18AC; // 6316  int32
    constexpr std::ptrdiff_t m_flFallbackWear        = 0x18B0; // 6320  float32
    constexpr std::ptrdiff_t m_nFallbackStatTrak     = 0x18B4; // 6324  int32

    // === C_AttributeContainer ================================== [OK] =======
    constexpr std::ptrdiff_t m_Item = 0x50; // 80  C_EconItemView

    // === C_EconItemView ======================================== [OK] =======
    // This struct did NOT move — all values match the external base exactly.
    constexpr std::ptrdiff_t m_iItemDefinitionIndex = 0x1BA; // 442  uint16
    constexpr std::ptrdiff_t m_iEntityQuality       = 0x1BC; // 444  int32
    constexpr std::ptrdiff_t m_iItemIDHigh          = 0x1D0; // 464  uint32
    constexpr std::ptrdiff_t m_iItemIDLow           = 0x1D4; // 468  uint32
    constexpr std::ptrdiff_t m_iAccountID           = 0x1D8; // 472  uint32

    // Not present in the external base's fallback table, but every other field
    // of C_EconItemView matched, so the struct layout is unchanged.  [STALE]
    constexpr std::ptrdiff_t m_bRestoreCustomMaterialAfterPrecache = 0x1B8;
    constexpr std::ptrdiff_t m_bInitialized                        = 0x1E8; // bool
    constexpr std::ptrdiff_t m_AttributeList                       = 0x208; // CAttributeList
    constexpr std::ptrdiff_t m_NetworkedDynamicAttributes          = 0x280;
    constexpr std::ptrdiff_t m_szCustomName                        = 0x2F8;
    constexpr std::ptrdiff_t m_szCustomNameOverride                = 0x399;

    // === CAttributeList ======================================== [STALE] ====
    constexpr std::ptrdiff_t m_Attributes = 0x8; // C_UtlVectorEmbeddedNetworkVar<CEconItemAttribute>

    // === CEconItemAttribute ==================================== [STALE] ====
    constexpr std::ptrdiff_t m_iAttributeDefinitionIndex = 0x30; // uint16
    constexpr std::ptrdiff_t m_flValue                   = 0x34; // float32
    constexpr std::ptrdiff_t m_flInitialValue            = 0x38; // float32
    constexpr std::ptrdiff_t m_nRefundableCurrency       = 0x3C; // int32
    constexpr std::ptrdiff_t m_bSetBonus                 = 0x40; // bool

    // === C_PlantedC4 =========================================== [OK] =======
    // m_flC4Blow moved +0x30; the rest of the block derived with that delta.
    constexpr std::ptrdiff_t m_bBombTicking      = 0x1288; // [DERIVED] +0x30
    constexpr std::ptrdiff_t m_flDefuseLength    = 0x12D4; // [DERIVED] +0x30
    constexpr std::ptrdiff_t m_flDefuseCountDown = 0x12D8; // [DERIVED] +0x30
    constexpr std::ptrdiff_t m_flC4Blow          = 0x12B8; // 4792
    constexpr std::ptrdiff_t m_bBombDefused      = 0x12DC; // 4828

    // === Bone / model ========================================== [OK] =======
    constexpr std::ptrdiff_t m_modelState    = 0x140; // 320   CSkeletonInstance
    constexpr std::ptrdiff_t m_materialGroup = 0x3F8; // 1016  CSkeletonInstance
    constexpr std::ptrdiff_t m_MeshGroupMask = 0x208; // 520   CModelState
    constexpr std::ptrdiff_t m_BoneArray     = 0x80;  // CModelState
    constexpr std::ptrdiff_t m_flSimulationTime = 0x3B8; // 952   C_BaseEntity

    // === Render overrides (C_BaseModelEntity) ================== [OK] =======
    constexpr std::ptrdiff_t m_nRenderMode            = 0xC80; // 3200  C_BaseModelEntity
    constexpr std::ptrdiff_t m_clrRender              = 0xCA0; // 3232  C_BaseModelEntity
    constexpr std::ptrdiff_t m_flGlowBackfaceMult     = 0xE40; // 3648  C_BaseModelEntity
    constexpr std::ptrdiff_t m_ClientOverrideTint     = 0x1048; // 4168  C_BaseModelEntity
    constexpr std::ptrdiff_t m_bUseClientOverrideTint = 0x104C; // 4172  C_BaseModelEntity

    // === C_CSPlayerPawn — gloves / misc ======================== [STALE] ====
    // No counterpart in the external base. skin_changer.h no longer reads the
    // active weapon through m_pClippingWeapon; it goes through
    // m_pWeaponServices -> m_hActiveWeapon, which is verified above.
    constexpr std::ptrdiff_t m_pClippingWeapon      = 0x3DC0;
    constexpr std::ptrdiff_t m_bNeedToReApplyGloves = 0x176D; // 5997  C_CSPlayerPawn
    constexpr std::ptrdiff_t m_EconGloves           = 0x1770; // 6000  C_CSPlayerPawn
    constexpr std::ptrdiff_t m_nEconGlovesChanged   = 0x1D20; // 7456  C_CSPlayerPawn
    constexpr std::ptrdiff_t m_hHudModelArms        = 0x1DA8; // 7592  C_CSPlayerPawn

    // === CCSPlayerController_InventoryServices ================= [OK] =======
    constexpr std::ptrdiff_t m_pInventoryServices = 0x820; // 2080  CCSPlayerController
    constexpr std::ptrdiff_t m_unMusicID          = 0x58; // 88    CCSPlayerController_InventoryServices

    // CCSGOInput vtable index for CreateMove (reference only, unused)
    constexpr int CCSGOInput_CreateMoveIdx = 21;

    // === Signature Patterns ==================================== [STALE] ====
    // Signatures survive build changes far better than offsets do, but these
    // were still written against the 2026-03-12 client. Every consumer must
    // treat a zero result as "feature off", never as "patch anyway".
    constexpr const char* sig_CreateMove_client = "48 8B C4 4C 89 40 18 48 89 48 08 55 53 41 54 41 55";
    constexpr const char* sig_ThirdPersonReset  = "48 8B 40 08 44 38 20 75 10 44 88 67 01";
    constexpr const char* sig_DrawObject        = "48 8B C4 53 57 41 54 48 81 EC D0 00 00 00 49 63 F9 49";
    constexpr const char* sig_LoadKV3           = "48 8D 0D ? ? ? ? FF 15 ? ? ? ? 49 8B 06";
    constexpr const char* sig_CreateMaterial    = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 8B F1 48 8B DA";
}
