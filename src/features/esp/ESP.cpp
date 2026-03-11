#include "../../Includes.h"

// -----------------------------------------------------------------------
// Helper: build a screen-space bounding box from feet/head positions
// -----------------------------------------------------------------------
bool ESP::GetBoundingBox(C_CSPlayerPawn* pPawn, ImVec2& vecMin, ImVec2& vecMax)
{
    CGameSceneNode* pSceneNode = pPawn->m_pGameSceneNode();
    if (!pSceneNode)
        return false;

    Vector vecOrigin = pSceneNode->m_vecAbsOrigin();

    float flHeadZ = 72.f;
    CCollisionProperty* pCollision = pPawn->m_pCollision();
    if (pCollision && reinterpret_cast<std::uintptr_t>(pCollision) > 0x10000)
    {
        Vector vecMaxs = pCollision->m_vecMaxs();
        if (vecMaxs.z > 10.f && vecMaxs.z < 100.f) flHeadZ = vecMaxs.z;
    }

    Vector vecHead = vecOrigin;
    vecHead.z += flHeadZ;

    ImVec2 screenFoot, screenHead;
    if (!Draw::WorldToScreen(vecOrigin, screenFoot) || !Draw::WorldToScreen(vecHead, screenHead))
        return false;

    float flHeight = screenFoot.y - screenHead.y;
    float flWidth  = flHeight * 0.5f;

    vecMin = ImVec2(screenFoot.x - flWidth * 0.5f, screenHead.y);
    vecMax = ImVec2(screenFoot.x + flWidth * 0.5f, screenFoot.y);
    return true;
}

// -----------------------------------------------------------------------
// Helper: pick colour based on team / visibility
// -----------------------------------------------------------------------
Color ESP::GetPlayerColor(CCSPlayerController* pController, C_CSPlayerPawn* pPawn)
{
    CCSPlayerController* pLocal = g_Globals.m_LocalPlayer.m_pController;
    if (!pLocal)
        return CONFIG_GET(Color, g_Variables.m_PlayerVisuals.m_colEnemyVisible);

    bool bSameTeam = (pPawn->m_iTeamNum() == g_Globals.m_LocalPlayer.m_pPlayerPawn->m_iTeamNum());
    if (bSameTeam)
        return CONFIG_GET(Color, g_Variables.m_PlayerVisuals.m_colTeammate);

    // simple spotted-state check for visibility
    bool bVisible = pPawn->m_entitySpottedState().m_bSpotted;
    return bVisible
        ? CONFIG_GET(Color, g_Variables.m_PlayerVisuals.m_colEnemyVisible)
        : CONFIG_GET(Color, g_Variables.m_PlayerVisuals.m_colEnemyOccluded);
}

// -----------------------------------------------------------------------
// Draw: 2-D solid rectangle box
// -----------------------------------------------------------------------
void ESP::DrawBox2D(const ImVec2& vecMin, const ImVec2& vecMax, const Color& col)
{
    bool bOutline = CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawBoxOutline);
    unsigned int uFlags = DRAW_RECT_NONE;
    if (bOutline) uFlags |= DRAW_RECT_OUTLINE;

    Draw::AddRect(vecMin, vecMax, col, uFlags, Color(0, 0, 0, 200));
}

// -----------------------------------------------------------------------
// Draw: corner box (only the four corners are drawn)
// -----------------------------------------------------------------------
void ESP::DrawBoxCorner(const ImVec2& vecMin, const ImVec2& vecMax, const Color& col)
{
    float flW = (vecMax.x - vecMin.x) * 0.25f;
    float flH = (vecMax.y - vecMin.y) * 0.25f;
    Color outline(0, 0, 0, 200);

    auto drawCorner = [&](float ox, float oy, float sx, float sy)
    {
        ImVec2 A(ox, oy);
        ImVec2 Bh(ox + flW * sx, oy);
        ImVec2 Bv(ox, oy + flH * sy);
        // outline (slightly offset)
        Draw::AddLine(ImVec2(A.x - sx, A.y - sy), ImVec2(Bh.x - sx, Bh.y - sy), outline, 3.0f);
        Draw::AddLine(ImVec2(A.x - sx, A.y - sy), ImVec2(Bv.x - sx, Bv.y - sy), outline, 3.0f);
        // colour
        Draw::AddLine(A, Bh, col, 1.5f);
        Draw::AddLine(A, Bv, col, 1.5f);
    };

    drawCorner(vecMin.x, vecMin.y,  1.f,  1.f); // top-left
    drawCorner(vecMax.x, vecMin.y, -1.f,  1.f); // top-right
    drawCorner(vecMin.x, vecMax.y,  1.f, -1.f); // bottom-left
    drawCorner(vecMax.x, vecMax.y, -1.f, -1.f); // bottom-right
}

// -----------------------------------------------------------------------
// Draw: health bar (vertical, left side)
// -----------------------------------------------------------------------
void ESP::DrawHealthBar(const ImVec2& vecMin, const ImVec2& vecMax, int iHealth, int iMaxHealth)
{
    if (iMaxHealth <= 0) iMaxHealth = 100;
    float flFrac   = static_cast<float>(iHealth) / static_cast<float>(iMaxHealth);
    flFrac         = std::clamp(flFrac, 0.f, 1.f);

    // green → yellow → red based on HP
    int r = static_cast<int>((1.f - flFrac) * 255.f);
    int g = static_cast<int>(flFrac         * 255.f);
    Color colHP(r, g, 0, 255);

    float flBarW = 4.f;
    float flPad  = 2.f;
    ImVec2 bgMin(vecMin.x - flBarW - flPad, vecMin.y);
    ImVec2 bgMax(vecMin.x - flPad,          vecMax.y);

    float flFilledY = bgMax.y - (bgMax.y - bgMin.y) * flFrac;

    // background
    Draw::AddRect(bgMin, bgMax, Color(0, 0, 0, 180), DRAW_RECT_FILLED);
    // filled portion
    Draw::AddRect(ImVec2(bgMin.x, flFilledY), bgMax, colHP, DRAW_RECT_FILLED);

    // HP number at bottom
    char szHP[8];
    snprintf(szHP, sizeof(szHP), "%d", iHealth);
    Draw::AddText(
        Fonts::ESP, Fonts::ESP->FontSize,
        ImVec2(bgMin.x - 1.f, bgMax.y + 1.f),
        szHP, Color(255, 255, 255, 220),
        DRAW_TEXT_DROPSHADOW, Color(0, 0, 0, 200)
    );
}

// -----------------------------------------------------------------------
// Draw: player name (top-center)
// -----------------------------------------------------------------------
void ESP::DrawName(const ImVec2& vecMin, const ImVec2& vecMax, const std::string& szName)
{
    if (szName.empty()) return;
    ImVec2 textSize = Fonts::ESP->CalcTextSizeA(Fonts::ESP->FontSize, FLT_MAX, 0.f, szName.c_str());
    float  cx       = (vecMin.x + vecMax.x) * 0.5f - textSize.x * 0.5f;
    Draw::AddText(Fonts::ESP, Fonts::ESP->FontSize,
        ImVec2(cx, vecMin.y - textSize.y - 1.f),
        szName, Color(255, 255, 255, 255),
        DRAW_TEXT_DROPSHADOW, Color(0, 0, 0, 220));
}

// -----------------------------------------------------------------------
// Draw: weapon name (bottom-center)
// -----------------------------------------------------------------------
static Color GetWeaponColor(const std::string& w)
{
    if (w == "ak47" || w == "m4a1" || w == "m4a1_silencer" || w == "aug" || w == "sg556" || w == "famas" || w == "galilar")
        return Color(255, 80, 80, 255);
    if (w == "awp" || w == "ssg08" || w == "scar20" || w == "g3sg1")
        return Color(255, 50, 200, 255);
    if (w == "mp9" || w == "mac10" || w == "mp7" || w == "mp5sd" || w == "ump45" || w == "p90" || w == "bizon")
        return Color(100, 200, 255, 255);
    if (w == "deagle" || w == "elite" || w == "fiveseven" || w == "glock" || w == "hkp2000" || w == "p250" || w == "tec9" || w == "usp_silencer" || w == "cz75a" || w == "revolver")
        return Color(255, 200, 50, 255);
    if (w == "nova" || w == "xm1014" || w == "sawedoff" || w == "mag7")
        return Color(255, 140, 50, 255);
    if (w == "m249" || w == "negev")
        return Color(200, 100, 255, 255);
    if (w.find("knife") != std::string::npos || w.find("bayonet") != std::string::npos)
        return Color(200, 200, 200, 255);
    if (w == "flashbang" || w == "hegrenade" || w == "smokegrenade" || w == "molotov" || w == "incgrenade" || w == "decoy")
        return Color(100, 255, 100, 255);
    return Color(200, 200, 100, 230);
}

void ESP::DrawWeapon(const ImVec2& vecMin, const ImVec2& vecMax, const std::string& szWeapon)
{
    if (szWeapon.empty()) return;

    Color colWeapon = GetWeaponColor(szWeapon);

    std::string szDisplay = szWeapon;
    std::transform(szDisplay.begin(), szDisplay.end(), szDisplay.begin(), ::toupper);

    ImVec2 textSize = Fonts::ESP->CalcTextSizeA(Fonts::ESP->FontSize, FLT_MAX, 0.f, szDisplay.c_str());
    float  cx = (vecMin.x + vecMax.x) * 0.5f - textSize.x * 0.5f;
    float  cy = vecMax.y + 2.f;

    Draw::AddText(Fonts::ESP, Fonts::ESP->FontSize,
        ImVec2(cx, cy),
        szDisplay, colWeapon,
        DRAW_TEXT_DROPSHADOW, Color(0, 0, 0, 200));
}

// -----------------------------------------------------------------------
// Draw: distance (below weapon name)
// -----------------------------------------------------------------------
void ESP::DrawDistance(const ImVec2& vecMin, const ImVec2& vecMax, float flDist)
{
    char szDist[16];
    snprintf(szDist, sizeof(szDist), "%.0fm", flDist / 52.49f); // units → metres
    ImVec2 textSize = Fonts::ESP->CalcTextSizeA(Fonts::ESP->FontSize, FLT_MAX, 0.f, szDist);
    float  cx       = (vecMin.x + vecMax.x) * 0.5f - textSize.x * 0.5f;
    Draw::AddText(Fonts::ESP, Fonts::ESP->FontSize,
        ImVec2(cx, vecMax.y + Fonts::ESP->FontSize + 3.f),
        szDist, Color(180, 180, 180, 200),
        DRAW_TEXT_DROPSHADOW, Color(0, 0, 0, 180));
}

// -----------------------------------------------------------------------
// Draw: head dot
// -----------------------------------------------------------------------
void ESP::DrawHeadDot(C_CSPlayerPawn* pPawn)
{
    CGameSceneNode* pNode = pPawn->m_pGameSceneNode();
    if (!pNode) return;
    Vector vecHead = pNode->m_vecAbsOrigin();
    CCollisionProperty* pCol = pPawn->m_pCollision();
    float flH = 72.f;
    if (pCol && reinterpret_cast<std::uintptr_t>(pCol) > 0x10000)
    {
        float fz = pCol->m_vecMaxs().z;
        if (fz > 10.f && fz < 100.f) flH = fz;
    }
    vecHead.z += flH;

    ImVec2 screenHead;
    if (!Draw::WorldToScreen(vecHead, screenHead)) return;
    Draw::AddCircle(screenHead, 3.f, Color(255, 255, 255, 220), 12, DRAW_CIRCLE_FILLED);
}

// -----------------------------------------------------------------------
// Draw: snapline from bottom-center of screen
// -----------------------------------------------------------------------
void ESP::DrawSnapline(const ImVec2& vecMin, const ImVec2& vecMax)
{
    ImVec2 origin(Window::m_iWidth * 0.5f, static_cast<float>(Window::m_iHeight));
    ImVec2 target((vecMin.x + vecMax.x) * 0.5f, vecMax.y);
    Draw::AddLine(origin, target, Color(255, 255, 100, 120));
}

// -----------------------------------------------------------------------
// Draw: skeleton (simplified – spine + arms + legs)
// Bone indices for CS2 may vary; this is a common approximate set.
// -----------------------------------------------------------------------
void ESP::DrawSkeleton(C_CSPlayerPawn* pPawn, const Color& col)
{
    CGameSceneNode* pNode = pPawn->m_pGameSceneNode();
    if (!pNode) return;

    BoneData_t* pBones = pNode->m_pBoneCache();
    if (!pBones || reinterpret_cast<std::uintptr_t>(pBones) < 0x1000) return;

    // pairs: { parent, child }
    static const std::pair<int,int> skeleton[] = {
        {6,  5},   // neck -> head
        {5,  4},   // spine top
        {4,  3},   // spine mid
        {3,  2},   // spine low
        {2,  0},   // pelvis
        // arms
        {5,  8},   // neck -> left shoulder
        {8,  9},   // left upper arm
        {9,  10},  // left forearm
        {5,  13},  // neck -> right shoulder
        {13, 14},  // right upper arm
        {14, 15},  // right forearm
        // legs
        {0,  22},  // pelvis -> left thigh
        {22, 23},  // left calf
        {23, 24},  // left foot
        {0,  25},  // pelvis -> right thigh
        {25, 26},  // right calf
        {26, 27},  // right foot
    };

    for (auto& [parent, child] : skeleton)
    {
        BoneData_t bParent = g_Memory.ReadMemory<BoneData_t>(reinterpret_cast<std::uintptr_t>(pBones) + parent * sizeof(BoneData_t));
        BoneData_t bChild  = g_Memory.ReadMemory<BoneData_t>(reinterpret_cast<std::uintptr_t>(pBones) + child  * sizeof(BoneData_t));

        ImVec2 scrParent, scrChild;
        if (!Draw::WorldToScreen(bParent.m_vecPosition, scrParent)) continue;
        if (!Draw::WorldToScreen(bChild.m_vecPosition,  scrChild))  continue;

        Draw::AddLine(scrParent, scrChild, col, 1.f);
    }
}

// -----------------------------------------------------------------------
// Main per-player render
// -----------------------------------------------------------------------
void ESP::RenderPlayer(CCSPlayerController* pController, C_CSPlayerPawn* pPawn)
{
    ImVec2 vecMin, vecMax;
    if (!GetBoundingBox(pPawn, vecMin, vecMax))
        return;

    Color col = GetPlayerColor(pController, pPawn);

    // ignore teammates?
    bool bIgnoreTeam = CONFIG_GET_ARRAY(bool, g_Variables.m_PlayerVisuals.m_vecVisualsModifiers, VISUALS_IGNORE_TEAMMATES);
    if (bIgnoreTeam && pPawn->m_iTeamNum() == g_Globals.m_LocalPlayer.m_pPlayerPawn->m_iTeamNum())
        return;

    int iBoxType = CONFIG_GET(int, g_Variables.m_PlayerVisuals.m_iBoxType);

    // --- Box ---
    if (CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawBox))
    {
        if (iBoxType == BOX_TYPE_2D || iBoxType == BOX_TYPE_BOTH)
            DrawBox2D(vecMin, vecMax, col);
        if (iBoxType == BOX_TYPE_CORNER || iBoxType == BOX_TYPE_BOTH)
            DrawBoxCorner(vecMin, vecMax, col);
    }

    // --- Health bar ---
    if (CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawHealthBar))
        DrawHealthBar(vecMin, vecMax, pPawn->m_iHealth(), pPawn->m_iMaxHealth());

    // --- Name ---
    if (CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawName))
        DrawName(vecMin, vecMax, pController->m_strSanitizedPlayerName());

    // --- Weapon ---
    if (CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawWeapon))
        DrawWeapon(vecMin, vecMax, pPawn->m_strActiveWeaponName());

    // --- Distance ---
    if (CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawDistance))
    {
        C_CSPlayerPawn* pLocal = g_Globals.m_LocalPlayer.m_pPlayerPawn;
        if (pLocal)
        {
            float flDist = (pPawn->m_pGameSceneNode()->m_vecAbsOrigin() - pLocal->m_pGameSceneNode()->m_vecAbsOrigin()).Length();
            DrawDistance(vecMin, vecMax, flDist);
        }
    }

    // --- Head dot ---
    if (CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawHeadDot))
        DrawHeadDot(pPawn);

    // --- Snapline ---
    if (CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawSnaplines))
        DrawSnapline(vecMin, vecMax);

    // --- Skeleton ---
    if (CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawSkeleton))
        DrawSkeleton(pPawn, Color(col, 160));
}
