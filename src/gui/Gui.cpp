#include "../Includes.h"

// -----------------------------------------------------------------------
// Hacker / Cyber color palette
// -----------------------------------------------------------------------
static inline ImVec4 C(int r, int g, int b, int a = 255)
{
    return ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

static void ApplyHackerTheme()
{
    ImGuiStyle& s = ImGui::GetStyle();

    s.WindowRounding    = 6.f;
    s.ChildRounding     = 4.f;
    s.FrameRounding     = 3.f;
    s.PopupRounding     = 4.f;
    s.ScrollbarRounding = 3.f;
    s.GrabRounding      = 3.f;
    s.TabRounding       = 3.f;

    s.WindowBorderSize  = 1.f;
    s.FrameBorderSize   = 1.f;
    s.PopupBorderSize   = 1.f;

    s.ItemSpacing       = ImVec2(8.f,  6.f);
    s.FramePadding      = ImVec2(6.f,  4.f);
    s.WindowPadding     = ImVec2(12.f, 10.f);

    ImVec4* c = s.Colors;

    c[ImGuiCol_WindowBg]            = C(10,  12,  18);
    c[ImGuiCol_ChildBg]             = C(14,  16,  24);
    c[ImGuiCol_PopupBg]             = C(12,  14,  20);

    c[ImGuiCol_Border]              = C(0,   180,  60, 80);
    c[ImGuiCol_BorderShadow]        = C(0,   0,    0,  0);

    c[ImGuiCol_FrameBg]             = C(18,  22,  32);
    c[ImGuiCol_FrameBgHovered]      = C(22,  28,  40);
    c[ImGuiCol_FrameBgActive]       = C(0,   200,  60, 60);

    c[ImGuiCol_TitleBg]             = C(8,   10,  16);
    c[ImGuiCol_TitleBgActive]       = C(0,   140,  50);
    c[ImGuiCol_TitleBgCollapsed]    = C(8,   10,  16);

    c[ImGuiCol_MenuBarBg]           = C(10,  12,  18);

    c[ImGuiCol_ScrollbarBg]         = C(10,  12,  18);
    c[ImGuiCol_ScrollbarGrab]       = C(0,   140,  50);
    c[ImGuiCol_ScrollbarGrabHovered]= C(0,   200,  70);
    c[ImGuiCol_ScrollbarGrabActive] = C(0,   255,  80);

    c[ImGuiCol_CheckMark]           = C(0,   255,  80);
    c[ImGuiCol_SliderGrab]          = C(0,   200,  60);
    c[ImGuiCol_SliderGrabActive]    = C(0,   255,  80);

    c[ImGuiCol_Button]              = C(0,   120,  45);
    c[ImGuiCol_ButtonHovered]       = C(0,   175,  60);
    c[ImGuiCol_ButtonActive]        = C(0,   255,  80);

    c[ImGuiCol_Header]              = C(0,   120,  45, 120);
    c[ImGuiCol_HeaderHovered]       = C(0,   175,  60, 180);
    c[ImGuiCol_HeaderActive]        = C(0,   220,  70);

    c[ImGuiCol_Separator]           = C(0,   120,  45, 80);
    c[ImGuiCol_SeparatorHovered]    = C(0,   180,  60);
    c[ImGuiCol_SeparatorActive]     = C(0,   255,  80);

    c[ImGuiCol_ResizeGrip]          = C(0,   120,  45, 80);
    c[ImGuiCol_ResizeGripHovered]   = C(0,   180,  60);
    c[ImGuiCol_ResizeGripActive]    = C(0,   255,  80);

    c[ImGuiCol_Tab]                 = C(10,  14,  22);
    c[ImGuiCol_TabHovered]          = C(0,   160,  55);
    c[ImGuiCol_TabActive]           = C(0,   200,  65);
    c[ImGuiCol_TabUnfocused]        = C(10,  14,  22);
    c[ImGuiCol_TabUnfocusedActive]  = C(0,   140,  50);

    c[ImGuiCol_PlotLines]           = C(0,   200,  60);
    c[ImGuiCol_PlotLinesHovered]    = C(0,   255,  80);
    c[ImGuiCol_PlotHistogram]       = C(0,   200,  60);
    c[ImGuiCol_PlotHistogramHovered]= C(0,   255,  80);

    c[ImGuiCol_TableBorderLight]    = C(0,   100,  40, 60);
    c[ImGuiCol_TableBorderStrong]   = C(0,   140,  50, 80);

    c[ImGuiCol_TextSelectedBg]      = C(0,   180,  60, 80);

    c[ImGuiCol_Text]                = C(200, 255, 210);
    c[ImGuiCol_TextDisabled]        = C(70,  100,  80);

    c[ImGuiCol_NavHighlight]        = C(0,   200,  65);
    c[ImGuiCol_DragDropTarget]      = C(0,   255,  80);
    c[ImGuiCol_ModalWindowDimBg]    = C(0,   0,    0, 120);
}

// -----------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------
static void SectionTitle(const char* label)
{
    ImGui::PushStyleColor(ImGuiCol_Text, C(0, 220, 70));
    ImGui::TextUnformatted(label);
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();
}

// Draw a red padlock + "MID+ only" message inline
static void LockedFeature(const char* szFeatureName, ETier eRequired)
{
    ImGui::PushStyleColor(ImGuiCol_Text, C(255, 60, 60));
    ImGui::Text("  [LOCKED]  %s  -  Requires %s tier",
        szFeatureName,
        eRequired == ETier::MID ? "MID" : "PRO");
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, C(120, 120, 140));
    ImGui::Text("  Upgrade at shifthub.uz");
    ImGui::PopStyleColor();
    ImGui::Spacing();
}

static void ColorEdit4Shim(const char* label, Color& col)
{
    float arr[4] = { col.rBase(), col.gBase(), col.bBase(), col.aBase() };
    if (ImGui::ColorEdit4(label, arr, ImGuiColorEditFlags_AlphaBar))
        col.Set(arr[0], arr[1], arr[2], arr[3]);
}

// Returns a human-readable key name for common VK codes
static const char* GetKeyName(int vk)
{
    switch (vk)
    {
    case 0:              return "Auto";
    case VK_LBUTTON:     return "LMB";
    case VK_RBUTTON:     return "RMB";
    case VK_MBUTTON:     return "MMB";
    case VK_XBUTTON1:    return "Mouse4";
    case VK_XBUTTON2:    return "Mouse5";
    case VK_SPACE:       return "Space";
    case VK_LSHIFT:      return "L.Shift";
    case VK_RSHIFT:      return "R.Shift";
    case VK_LCONTROL:    return "L.Ctrl";
    case VK_RCONTROL:    return "R.Ctrl";
    case VK_LMENU:       return "L.Alt";
    case VK_RMENU:       return "R.Alt";
    case VK_CAPITAL:     return "CapsLock";
    case VK_TAB:         return "Tab";
    case VK_F1:          return "F1";
    case VK_F2:          return "F2";
    case VK_F3:          return "F3";
    case VK_F4:          return "F4";
    case VK_F5:          return "F5";
    case VK_F6:          return "F6";
    case VK_INSERT:      return "Insert";
    case VK_DELETE:      return "Delete";
    default:
        // Single printable character (A-Z, 0-9)
        if (vk >= 'A' && vk <= 'Z') { static char buf[2]; buf[0] = (char)vk; buf[1] = 0; return buf; }
        if (vk >= '0' && vk <= '9') { static char buf[2]; buf[0] = (char)vk; buf[1] = 0; return buf; }
        return "?";
    }
}

// Shows current key + a "[ Bind ]" button. Click Bind, then press any key to assign.
// id must be unique (e.g. "##bind_aimkey")
static void KeyBind(const char* label, int& key, const char* id)
{
    static int*  s_pListening  = nullptr; // which key slot is waiting for input
    static float s_flListenTimer = 0.f;   // cancel after 5s

    bool bThisListening = (s_pListening == &key);

    ImGui::Text("%s", label);
    ImGui::SameLine();

    if (bThisListening)
    {
        s_flListenTimer -= ImGui::GetIO().DeltaTime;
        if (s_flListenTimer <= 0.f)
            s_pListening = nullptr;

        ImGui::PushStyleColor(ImGuiCol_Button,        C(180, 60,  0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C(220, 80,  0));
        ImGui::PushStyleColor(ImGuiCol_Text,          C(255, 255, 100));

        char buf[32];
        snprintf(buf, sizeof(buf), "[ Press key... ]##%s", id);
        ImGui::Button(buf, ImVec2(140.f, 0.f));
        ImGui::PopStyleColor(3);

        // Scan all keys
        for (int i = 1; i < 256; i++)
        {
            if (i == VK_ESCAPE)         { s_pListening = nullptr; break; }  // ESC = cancel
            if (GetAsyncKeyState(i) & 0x0001)
            {
                key = i;
                s_pListening = nullptr;
                break;
            }
        }
    }
    else
    {
        // Show current key name + Bind button
        char btnLabel[64];
        snprintf(btnLabel, sizeof(btnLabel), "[ %-8s ] Bind##%s", GetKeyName(key), id);

        ImGui::PushStyleColor(ImGuiCol_Button,        C(20, 60, 30));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C(30, 90, 45));
        if (ImGui::Button(btnLabel, ImVec2(160.f, 0.f)))
        {
            s_pListening   = &key;
            s_flListenTimer = 5.f;
        }
        ImGui::PopStyleColor(2);
    }

    // "0 = Auto" hint for trigger key
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, C(80, 80, 100));
    ImGui::Text("(ESC=cancel)");
    ImGui::PopStyleColor();
}

// -----------------------------------------------------------------------
// Initialize fonts
// -----------------------------------------------------------------------
void Gui::Initialize(unsigned int uFontFlags)
{
    ImGuiIO& io = ImGui::GetIO();

    ApplyHackerTheme();

    ImFontConfig cfg = {};
    cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_LightHinting
                         | ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_Bold;

    Fonts::Default = io.Fonts->AddFontFromFileTTF(X("C:\\Windows\\Fonts\\Verdana.ttf"), 18, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    Fonts::ESP      = io.Fonts->AddFontFromFileTTF(X("C:\\Windows\\Fonts\\Verdana.ttf"), 10, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    m_bInitialized = ImGuiFreeType::BuildFontAtlas(io.Fonts, uFontFlags);
}

// -----------------------------------------------------------------------
// Update
// -----------------------------------------------------------------------
void Gui::Update(ImGuiIO& io)
{
    io.MouseDrawCursor = m_bOpen;
    if (m_bOpen)
    {
        POINT p;
        if (GetCursorPos(&p))
            io.MousePos = ImVec2(static_cast<float>(p.x), static_cast<float>(p.y));
    }
}

// -----------------------------------------------------------------------
// Render
// -----------------------------------------------------------------------
void Gui::Render()
{
    if (!m_bInitialized)
        return;

    ImGuiIO& io = ImGui::GetIO();
    Gui::Update(io);

    // ================================================================
    // Title bar text:  SHIFTHUB  |  v1.0  |  [TIER]  |  User
    // ================================================================
    char szTitle[128];
    snprintf(szTitle, sizeof(szTitle),
        " SHIFTHUB.UZ  |  v1.0  |  [ %s ]  |  %s",
        g_License.GetTierName(),
        g_License.m_strUser.c_str());

    ImGui::SetNextWindowSize(ImVec2(880, 560));
    ImGui::Begin(szTitle, nullptr,
        ImGuiWindowFlags_NoResize       |
        ImGuiWindowFlags_NoSavedSettings|
        ImGuiWindowFlags_NoCollapse);
    {
        // Tier badge strip
        ImGui::PushStyleColor(ImGuiCol_Text, g_License.GetTierColor());
        ImGui::Text("  Tier: %s   |   %s   |   shifthub.uz",
            g_License.GetTierName(),
            g_License.m_strExpiry.c_str());
        ImGui::PopStyleColor();

        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::BeginTabBar(X("##MainTabs")))
        {
            // ============================================================
            // VISUALS  (all tiers)
            // ============================================================
            if (ImGui::BeginTabItem(X("[ VISUALS ]")))
            {
                ImGui::Spacing();
                SectionTitle("Player ESP");

                ImGui::Checkbox(X("Enable ESP"), &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bEnableVisuals));

                if (CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bEnableVisuals))
                {
                    ImGui::Spacing();
                    {
                        auto& vecMod = Config::Get<std::vector<bool>>(g_Variables.m_PlayerVisuals.m_vecVisualsModifiers);
                        bool bIgnoreTeam = vecMod[VISUALS_IGNORE_TEAMMATES];
                        bool bOnlyVis    = vecMod[VISUALS_ONLY_WHEN_VISIBLE];
                        if (ImGui::Checkbox(X("Ignore Teammates"), &bIgnoreTeam)) vecMod[VISUALS_IGNORE_TEAMMATES] = bIgnoreTeam;
                        ImGui::SameLine(220.f);
                        if (ImGui::Checkbox(X("Only Visible"), &bOnlyVis))        vecMod[VISUALS_ONLY_WHEN_VISIBLE] = bOnlyVis;
                    }

                    ImGui::Spacing();
                    SectionTitle("Box");

                    ImGui::Checkbox(X("Draw Box"), &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawBox));
                    if (CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawBox))
                    {
                        static const char* boxTypes[] = { "2D Box", "Corner Box", "Both" };
                        ImGui::SetNextItemWidth(160.f);
                        ImGui::Combo(X("Box Type"), &CONFIG_GET(int, g_Variables.m_PlayerVisuals.m_iBoxType), boxTypes, 3);
                        ImGui::Checkbox(X("Box Outline"), &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawBoxOutline));
                    }

                    ImGui::Spacing();
                    SectionTitle("Overlays");

                    ImGui::Checkbox(X("Health Bar"),  &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawHealthBar));
                    ImGui::SameLine(220.f);
                    ImGui::Checkbox(X("Player Name"), &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawName));

                    ImGui::Checkbox(X("Weapon Name"), &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawWeapon));
                    ImGui::SameLine(220.f);
                    ImGui::Checkbox(X("Distance"),    &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawDistance));

                    ImGui::Checkbox(X("Skeleton"),    &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawSkeleton));
                    ImGui::SameLine(220.f);
                    ImGui::Checkbox(X("Head Dot"),    &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawHeadDot));

                    ImGui::Checkbox(X("Snap Lines"),  &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawSnaplines));

                ImGui::Spacing();
                SectionTitle("Hotkey");

                KeyBind("ESP Toggle:", CONFIG_GET(int, g_Variables.m_Hotkeys.m_iESPToggleKey), "espkey");

                ImGui::Spacing();
                SectionTitle("Colors");

                    ColorEdit4Shim(X("Enemy Visible"),  CONFIG_GET(Color, g_Variables.m_PlayerVisuals.m_colEnemyVisible));
                    ColorEdit4Shim(X("Enemy Occluded"), CONFIG_GET(Color, g_Variables.m_PlayerVisuals.m_colEnemyOccluded));
                    ColorEdit4Shim(X("Teammate"),       CONFIG_GET(Color, g_Variables.m_PlayerVisuals.m_colTeammate));
                }

                ImGui::EndTabItem();
            }

            // ============================================================
            // MOVEMENT  (MID+)
            // ============================================================
            if (ImGui::BeginTabItem(X("[ MOVEMENT ]")))
            {
                ImGui::Spacing();

                if (g_License.HasFeature(ETier::MID))
                {
                    SectionTitle("Bunny Hop");

                    ImGui::Checkbox(X("Enable Bhop"), &CONFIG_GET(bool, g_Variables.m_Bhop.m_bEnableBhop));

                    if (CONFIG_GET(bool, g_Variables.m_Bhop.m_bEnableBhop))
                    {
                        ImGui::Spacing();
                        ImGui::PushStyleColor(ImGuiCol_Text, C(255, 200, 50));
                        ImGui::Text("  Space (32) ishlatmang!");
                        ImGui::PopStyleColor();
                        KeyBind("Hold Key:", CONFIG_GET(int, g_Variables.m_Bhop.m_iBhopKey), "bhopkey");
                        ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                        ImGui::Text("  Bu tugmani ushlab turing — cheat Space bosadi");
                        ImGui::PopStyleColor();
                    }
                }
                else
                {
                    LockedFeature("Bunny Hop", ETier::MID);
                }

                ImGui::EndTabItem();
            }

            // ============================================================
            // COMBAT  (MID: Triggerbot | PRO: Aimbot)
            // ============================================================
            if (ImGui::BeginTabItem(X("[ COMBAT ]")))
            {
                ImGui::Spacing();

                // --- Triggerbot (MID+) ---
                SectionTitle("Triggerbot");

                if (g_License.HasFeature(ETier::MID))
                {
                    ImGui::Checkbox(X("Enable Triggerbot"), &CONFIG_GET(bool, g_Variables.m_TriggerBot.m_bEnableTriggerbot));
                    ImGui::Spacing();

                    KeyBind("Hold Key:", CONFIG_GET(int, g_Variables.m_TriggerBot.m_iTriggerKey), "trigkey");
                    ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                    ImGui::Text("  Auto (0) = hech narsa ushlamay otadi");
                    ImGui::PopStyleColor();

                    ImGui::SetNextItemWidth(200.f);
                    ImGui::SliderFloat(X("Shot Delay"), &CONFIG_GET(float, g_Variables.m_TriggerBot.m_flShotDelay), 0.f, 300.f, "%.0f ms");

                    ImGui::Checkbox(X("Ignore Teammates##trig"), &CONFIG_GET(bool, g_Variables.m_TriggerBot.m_bIgnoreTeammates));
                    ImGui::Checkbox(X("Only Visible (no wallbang)"), &CONFIG_GET(bool, g_Variables.m_TriggerBot.m_bOnlyVisible));
                }
                else
                {
                    LockedFeature("Triggerbot", ETier::MID);
                }

                ImGui::Spacing();
                ImGui::Spacing();

                // --- Aimbot (PRO) ---
                SectionTitle("Aimbot");

                if (g_License.HasFeature(ETier::PRO))
                {
                    ImGui::Checkbox(X("Enable Aimbot"), &CONFIG_GET(bool, g_Variables.m_AimBot.m_bEnableAimbot));
                    ImGui::Spacing();

                    KeyBind("Aim Key:", CONFIG_GET(int, g_Variables.m_AimBot.m_iAimKey), "aimkey");

                    ImGui::SetNextItemWidth(200.f);
                    ImGui::SliderFloat(X("FOV"), &CONFIG_GET(float, g_Variables.m_AimBot.m_flFOV), 0.5f, 30.f, "%.1f deg");

                    ImGui::SetNextItemWidth(200.f);
                    ImGui::SliderFloat(X("Smooth"), &CONFIG_GET(float, g_Variables.m_AimBot.m_flSmooth), 1.f, 20.f, "%.1f");
                    ImGui::PushStyleColor(ImGuiCol_Text, C(120, 120, 140));
                    ImGui::SameLine(); ImGui::Text("(1=instant, 10=smooth)");
                    ImGui::PopStyleColor();

                    static const char* hitboxNames[] = { "Head", "Neck", "Chest" };
                    ImGui::SetNextItemWidth(140.f);
                    ImGui::Combo(X("Hitbox"), &CONFIG_GET(int, g_Variables.m_AimBot.m_iHitbox), hitboxNames, 3);

                    ImGui::Checkbox(X("Ignore Teammates##aim"), &CONFIG_GET(bool, g_Variables.m_AimBot.m_bIgnoreTeammates));
                    ImGui::Checkbox(X("Draw FOV Circle"),       &CONFIG_GET(bool, g_Variables.m_AimBot.m_bDrawFOV));
                }
                else
                {
                    LockedFeature("Aimbot", ETier::PRO);
                }

                ImGui::EndTabItem();
            }

            // ============================================================
            // RADAR  (all tiers)
            // ============================================================
            if (ImGui::BeginTabItem(X("[ RADAR ]")))
            {
                ImGui::Spacing();
                SectionTitle("2D Radar");

                ImGui::Checkbox(X("Enable Radar"), &CONFIG_GET(bool, g_Variables.m_Radar.m_bEnableRadar));

                if (CONFIG_GET(bool, g_Variables.m_Radar.m_bEnableRadar))
                {
                    ImGui::Spacing();
                    ImGui::SetNextItemWidth(180.f);
                    ImGui::SliderFloat(X("Radar Size"),   &CONFIG_GET(float, g_Variables.m_Radar.m_flRadarSize),  100.f, 400.f, "%.0f px");
                    ImGui::SetNextItemWidth(180.f);
                    ImGui::SliderFloat(X("Radar Range"),  &CONFIG_GET(float, g_Variables.m_Radar.m_flRadarRange), 500.f, 5000.f, "%.0f u");
                    ImGui::SetNextItemWidth(180.f);
                    ImGui::SliderFloat(X("Position X"),   &CONFIG_GET(float, g_Variables.m_Radar.m_flRadarX),    0.f, 1920.f, "%.0f");
                    ImGui::SetNextItemWidth(180.f);
                    ImGui::SliderFloat(X("Position Y"),   &CONFIG_GET(float, g_Variables.m_Radar.m_flRadarY),    0.f, 1080.f, "%.0f");
                    ImGui::Checkbox(X("Rotate with View"), &CONFIG_GET(bool, g_Variables.m_Radar.m_bRadarRotate));
                }

                ImGui::EndTabItem();
            }

            // ============================================================
            // CONFIGS  (all tiers)
            // ============================================================
            if (ImGui::BeginTabItem(X("[ CONFIGS ]")))
            {
                ImGui::Spacing();
                SectionTitle("Config Manager");

                ImGui::BeginColumns(X("##CfgCols"), 2, ImGuiColumnsFlags_NoResize);
                {
                    static int nSelected = -1;
                    if (ImGui::BeginListBox(X("##CfgList"), ImVec2(-1, -1)))
                    {
                        for (size_t i = 0; i < Config::m_vecFileNames.size(); i++)
                        {
                            if (ImGui::Selectable(Config::m_vecFileNames.at(i).c_str(),
                                i == (size_t)nSelected,
                                ImGuiSelectableFlags_DontClosePopups))
                                nSelected = (int)i;
                        }
                        ImGui::EndListBox();
                    }
                    ImGui::NextColumn();

                    static std::string strCfgName;
                    ImGui::InputTextWithHint(X("##cfgfile"), X("config name..."), &strCfgName);

                    if (ImGui::Button(X("Create"), ImVec2(120.f, 0.f)))
                    {
                        Config::Save(strCfgName);
                        strCfgName.clear();
                        Config::Refresh();
                    }
                    if (ImGui::Button(X("Refresh"), ImVec2(120.f, 0.f))) Config::Refresh();

                    if (nSelected != -1)
                    {
                        ImGui::Spacing();
                        if (ImGui::Button(X("Save"),   ImVec2(120.f, 0.f))) Config::Save(Config::m_vecFileNames.at(nSelected));
                        if (ImGui::Button(X("Load"),   ImVec2(120.f, 0.f))) Config::Load(Config::m_vecFileNames.at(nSelected));
                        if (ImGui::Button(X("Delete"), ImVec2(120.f, 0.f)))
                        {
                            Config::Remove(Config::m_vecFileNames.at(nSelected));
                            Config::Refresh();
                            nSelected = -1;
                        }
                    }
                }
                ImGui::EndColumns();
                ImGui::EndTabItem();
            }

            // ============================================================
            // SETTINGS  (all tiers)
            // ============================================================
            if (ImGui::BeginTabItem(X("[ SETTINGS ]")))
            {
                ImGui::Spacing();
                SectionTitle("General");

                ImGui::Text("Menu Key (VK hex):");
                ImGui::SetNextItemWidth(120.f);
                ImGui::InputInt(X("##menukey"), &CONFIG_GET(int, g_Variables.m_Gui.m_iMenuKey));

                ImGui::Text("Unload Key (VK hex):");
                ImGui::SetNextItemWidth(120.f);
                ImGui::InputInt(X("##unloadkey"), &CONFIG_GET(int, g_Variables.m_Gui.m_iUnloadKey));

                ImGui::Checkbox(X("Exclude from Desktop Capture"),
                    &CONFIG_GET(bool, g_Variables.m_Gui.m_bExcludeFromDesktopCapture));

                ImGui::Spacing();
                ImGui::Spacing();
                SectionTitle("Misc Features");

                ImGui::Checkbox(X("Anti-Flash"), &CONFIG_GET(bool, g_Variables.m_Misc.m_bAntiFlash));
                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                ImGui::Text("  Flash grenade effektini yo'q qiladi");
                ImGui::PopStyleColor();

                ImGui::Checkbox(X("C4 Timer"), &CONFIG_GET(bool, g_Variables.m_Misc.m_bC4Timer));
                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                ImGui::Text("  Bomba portlashigacha qolgan vaqtni ko'rsatadi");
                ImGui::PopStyleColor();

                ImGui::Spacing();
                ImGui::Spacing();
                SectionTitle("License Info");

                ImGui::PushStyleColor(ImGuiCol_Text, g_License.GetTierColor());
                ImGui::Text("  Tier:    %s", g_License.GetTierName());
                ImGui::Text("  User:    %s", g_License.m_strUser.c_str());
                ImGui::Text("  Status:  %s", g_License.m_strExpiry.c_str());
                ImGui::PopStyleColor();

                ImGui::Spacing();
                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 120, 110));
                ImGui::Text("  To upgrade place a key.txt file next to the exe.");
                ImGui::Text("  Key format:  SH-XXXXXXXX-M  (MID)  or  SH-XXXXXXXX-P  (PRO)");
                ImGui::Text("  Buy keys at: shifthub.uz");
                ImGui::PopStyleColor();

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}
