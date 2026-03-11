#include "../../Includes.h"

static constexpr int BONE_HEAD  = 6;
static constexpr int BONE_NECK  = 5;
static constexpr int BONE_CHEST = 4;

// -----------------------------------------------------------------------
Vector Aimbot::GetHitboxPosition(C_CSPlayerPawn* pPawn, int iHitbox)
{
    CGameSceneNode* pNode = pPawn->m_pGameSceneNode();
    if (!pNode) return {};

    BoneData_t* pBones = pNode->m_pBoneCache();
    if (!pBones) return {};

    int iBone = BONE_HEAD;
    if (iHitbox == 1) iBone = BONE_NECK;
    else if (iHitbox == 2) iBone = BONE_CHEST;

    BoneData_t bone = g_Memory.ReadMemory<BoneData_t>(
        reinterpret_cast<std::uintptr_t>(pBones) + iBone * sizeof(BoneData_t));
    return bone.m_vecPosition;
}

// -----------------------------------------------------------------------
QAngle Aimbot::SmoothAngle(const QAngle& angCurrent, const QAngle& angTarget, float flSmooth)
{
    if (flSmooth <= 1.f) return angTarget;

    QAngle angDelta = angTarget - angCurrent;
    angDelta.Normalize();

    return angCurrent + (angDelta / flSmooth);
}

// -----------------------------------------------------------------------
C_CSPlayerPawn* Aimbot::FindBestTarget(const std::vector<EntityObject_t>& vecEntities, float& flBestFOV)
{
    C_CSPlayerPawn* pBest = nullptr;
    flBestFOV = 999999.f;

    C_CSPlayerPawn* pLocalPawn = g_Globals.m_LocalPlayer.m_pPlayerPawn;
    if (!pLocalPawn) return nullptr;

    float flMaxFOV = CONFIG_GET(float, g_Variables.m_AimBot.m_flFOV);
    bool  bIgnoreTeam = CONFIG_GET(bool, g_Variables.m_AimBot.m_bIgnoreTeammates);

    // Screen center = crosshair position
    float flScreenCX = Window::m_iWidth  * 0.5f;
    float flScreenCY = Window::m_iHeight * 0.5f;

    // Convert FOV degrees to screen pixel radius
    float flFOVRadius = std::tan(M_DEG2RAD(flMaxFOV) * 0.5f) * static_cast<float>(Window::m_iHeight);
    if (flFOVRadius <= 0.f) return nullptr;

    for (const EntityObject_t& obj : vecEntities)
    {
        if (obj.m_eType != EEntityType::ENTITY_PLAYER) continue;

        CCSPlayerController* pCtrl = reinterpret_cast<CCSPlayerController*>(obj.m_pEntity);
        if (!pCtrl || pCtrl->m_bIsLocalPlayerController()) continue;

        C_CSPlayerPawn* pPawn = reinterpret_cast<C_CSPlayerPawn*>(pCtrl->m_hPawn().Get());
        if (!pPawn || !pPawn->IsAlive()) continue;

        if (bIgnoreTeam && pPawn->m_iTeamNum() == pLocalPawn->m_iTeamNum()) continue;

        Vector vecTarget = GetHitboxPosition(pPawn, CONFIG_GET(int, g_Variables.m_AimBot.m_iHitbox));
        if (vecTarget.IsZero()) continue;

        // Project hitbox to screen
        ImVec2 screenPos;
        if (!Draw::WorldToScreen(vecTarget, screenPos)) continue;

        float flDX = screenPos.x - flScreenCX;
        float flDY = screenPos.y - flScreenCY;
        float flDist = std::sqrtf(flDX * flDX + flDY * flDY);

        // Only targets within FOV circle
        if (flDist > flFOVRadius) continue;

        if (flDist < flBestFOV)
        {
            flBestFOV = flDist;
            pBest = pPawn;
        }
    }
    return pBest;
}

// -----------------------------------------------------------------------
void Aimbot::DrawFOVCircle()
{
    if (!CONFIG_GET(bool, g_Variables.m_AimBot.m_bDrawFOV)) return;

    float flFOV = CONFIG_GET(float, g_Variables.m_AimBot.m_flFOV);
    float flW = static_cast<float>(Window::m_iWidth);
    float flH = static_cast<float>(Window::m_iHeight);
    ImVec2 center(flW * 0.5f, flH * 0.5f);

    float flRadius = std::tan(M_DEG2RAD(flFOV) * 0.5f) * flH;
    if (flRadius <= 0.f || !std::isfinite(flRadius)) return;

    Draw::AddCircle(center, flRadius, Color(255, 255, 255, 80), 64, DRAW_CIRCLE_NONE);
}

// -----------------------------------------------------------------------
void Aimbot::Run(const std::vector<EntityObject_t>& vecEntities)
{
    int iAimKey = CONFIG_GET(int, g_Variables.m_AimBot.m_iAimKey);
    if (!(GetAsyncKeyState(iAimKey) & 0x8000)) return;

    C_CSPlayerPawn* pLocalPawn = g_Globals.m_LocalPlayer.m_pPlayerPawn;
    if (!pLocalPawn) return;

    float flBestDist;
    C_CSPlayerPawn* pTarget = FindBestTarget(vecEntities, flBestDist);
    if (!pTarget) return;

    Vector vecTarget = GetHitboxPosition(pTarget, CONFIG_GET(int, g_Variables.m_AimBot.m_iHitbox));
    if (vecTarget.IsZero()) return;

    // Project target hitbox to screen
    ImVec2 screenTarget;
    if (!Draw::WorldToScreen(vecTarget, screenTarget)) return;

    float flScreenCX = Window::m_iWidth  * 0.5f;
    float flScreenCY = Window::m_iHeight * 0.5f;

    float flDeltaX = screenTarget.x - flScreenCX;
    float flDeltaY = screenTarget.y - flScreenCY;

    if (!std::isfinite(flDeltaX) || !std::isfinite(flDeltaY)) return;

    // Apply smoothing (divide pixel delta by smooth factor)
    float flSmooth = CONFIG_GET(float, g_Variables.m_AimBot.m_flSmooth);
    if (flSmooth < 1.f) flSmooth = 1.f;
    flDeltaX /= flSmooth;
    flDeltaY /= flSmooth;

    // Clamp to prevent crazy jumps
    flDeltaX = std::clamp(flDeltaX, -150.f, 150.f);
    flDeltaY = std::clamp(flDeltaY, -150.f, 150.f);

    // Skip tiny movements (already on target)
    if (std::fabsf(flDeltaX) < 0.5f && std::fabsf(flDeltaY) < 0.5f) return;

    // Use mouse_event — works with CS2 Raw Input (unlike SendInput)
    mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(flDeltaX), static_cast<DWORD>(flDeltaY), 0, 0);
}
