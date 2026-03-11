#include "../../Includes.h"

// -----------------------------------------------------------------------
// Minimap radar rendered via ImGui::GetForegroundDrawList
// -----------------------------------------------------------------------
void Radar::Render(const std::vector<EntityObject_t>& vecEntities)
{
    float flSize  = CONFIG_GET(float, g_Variables.m_Radar.m_flRadarSize);
    float flRange = CONFIG_GET(float, g_Variables.m_Radar.m_flRadarRange);
    float flX     = CONFIG_GET(float, g_Variables.m_Radar.m_flRadarX);
    float flY     = CONFIG_GET(float, g_Variables.m_Radar.m_flRadarY);
    bool  bRotate = CONFIG_GET(bool,  g_Variables.m_Radar.m_bRadarRotate);

    ImDrawList* pDL = ImGui::GetForegroundDrawList();

    // Background
    ImVec2 bgMin(flX, flY);
    ImVec2 bgMax(flX + flSize, flY + flSize);
    pDL->AddRectFilled(bgMin, bgMax, IM_COL32(10, 10, 10, 180), 4.f);
    pDL->AddRect(bgMin, bgMax, IM_COL32(80, 80, 80, 200), 4.f, 0, 1.5f);

    // Cross-hair lines
    ImVec2 center(flX + flSize * 0.5f, flY + flSize * 0.5f);
    pDL->AddLine(ImVec2(center.x, flY),         ImVec2(center.x, flY + flSize), IM_COL32(60,60,60,120), 1.f);
    pDL->AddLine(ImVec2(flX, center.y),          ImVec2(flX + flSize, center.y), IM_COL32(60,60,60,120), 1.f);

    // Local player
    C_CSPlayerPawn* pLocalPawn = g_Globals.m_LocalPlayer.m_pPlayerPawn;
    if (!pLocalPawn) return;

    CGameSceneNode* pLocalNode = pLocalPawn->m_pGameSceneNode();
    if (!pLocalNode) return;

    Vector vecLocalPos = pLocalNode->m_vecAbsOrigin();
    QAngle angView     = g_Interfaces.m_CSGOInput.m_angViewAngle;
    float  flYawRad    = M_DEG2RAD(bRotate ? angView.y : 0.f);

    // Draw local player marker (white triangle pointing forward)
    {
        float sz = 5.f;
        float fwd = bRotate ? -(float)(M_PI / 2.0) : M_DEG2RAD(-angView.y + 90.f);
        ImVec2 tip(center.x + sz * std::cosf(fwd),      center.y + sz * std::sinf(fwd));
        ImVec2 lft(center.x + sz * std::cosf(fwd + 2.5f), center.y + sz * std::sinf(fwd + 2.5f));
        ImVec2 rgt(center.x + sz * std::cosf(fwd - 2.5f), center.y + sz * std::sinf(fwd - 2.5f));
        pDL->AddTriangleFilled(tip, lft, rgt, IM_COL32(255, 255, 255, 255));
    }

    float flScale = (flSize * 0.5f) / flRange;

    for (const EntityObject_t& obj : vecEntities)
    {
        if (obj.m_eType != EEntityType::ENTITY_PLAYER) continue;

        CCSPlayerController* pCtrl = reinterpret_cast<CCSPlayerController*>(obj.m_pEntity);
        if (!pCtrl || pCtrl->m_bIsLocalPlayerController()) continue;

        C_CSPlayerPawn* pPawn = reinterpret_cast<C_CSPlayerPawn*>(pCtrl->m_hPawn().Get());
        if (!pPawn || !pPawn->IsAlive()) continue;

        CGameSceneNode* pNode = pPawn->m_pGameSceneNode();
        if (!pNode) continue;

        Vector vecPos = pNode->m_vecAbsOrigin();
        float  dx     = vecPos.x - vecLocalPos.x;
        float  dy     = vecPos.y - vecLocalPos.y;

        // Optionally rotate around local player view
        float rdx = dx, rdy = dy;
        if (bRotate)
        {
            float cosA = std::cosf(-flYawRad);
            float sinA = std::sinf(-flYawRad);
            rdx = dx * cosA - dy * sinA;
            rdy = dx * sinA + dy * cosA;
        }

        float screenX = center.x + rdx * flScale;
        float screenY = center.y - rdy * flScale; // y is flipped

        // Clamp inside radar circle area
        float cx = screenX - center.x;
        float cy = screenY - center.y;
        float dist = std::sqrtf(cx * cx + cy * cy);
        float maxR  = flSize * 0.5f - 6.f;
        if (dist > maxR)
        {
            float norm = maxR / dist;
            screenX = center.x + cx * norm;
            screenY = center.y + cy * norm;
        }

        bool bSameTeam = (pPawn->m_iTeamNum() == pLocalPawn->m_iTeamNum());
        ImU32 colDot   = bSameTeam ? IM_COL32(50, 220, 50, 255) : IM_COL32(220, 50, 50, 255);

        pDL->AddCircleFilled(ImVec2(screenX, screenY), 4.f, colDot);
        pDL->AddCircle(ImVec2(screenX, screenY), 4.f, IM_COL32(0, 0, 0, 200));

        // Player name (small)
        std::string szName = pCtrl->m_strSanitizedPlayerName();
        if (!szName.empty() && Fonts::ESP)
            pDL->AddText(Fonts::ESP, 8.f, ImVec2(screenX + 5.f, screenY - 4.f),
                         colDot, szName.c_str());
    }
}
