#include "../Includes.h"

// -----------------------------------------------------------------------
// Knife skin gradient preview renderer (V2.0)
// Draws a color gradient representing each knife skin pattern
// -----------------------------------------------------------------------
struct KnifeSkinColors_t
{
    int m_nPaintKit;
    ImU32 m_colLeft;
    ImU32 m_colMid;
    ImU32 m_colRight;
};

static const KnifeSkinColors_t g_KnifeGradients[] = {
    // Doppler series
    { 415,  IM_COL32(20, 10, 60, 255),    IM_COL32(80, 20, 140, 255),   IM_COL32(30, 30, 180, 255)  },  // Doppler Phase 1
    { 416,  IM_COL32(200, 50, 120, 255),   IM_COL32(120, 30, 180, 255),  IM_COL32(50, 20, 100, 255)  },  // Doppler Phase 2
    { 417,  IM_COL32(10, 40, 30, 255),     IM_COL32(20, 80, 60, 255),    IM_COL32(40, 40, 120, 255)  },  // Doppler Phase 3
    { 418,  IM_COL32(20, 60, 180, 255),    IM_COL32(10, 30, 100, 255),   IM_COL32(60, 20, 160, 255)  },  // Doppler Phase 4
    { 419,  IM_COL32(180, 10, 10, 255),    IM_COL32(220, 30, 30, 255),   IM_COL32(140, 5, 5, 255)    },  // Doppler Ruby
    { 420,  IM_COL32(10, 30, 200, 255),    IM_COL32(20, 60, 255, 255),   IM_COL32(5, 20, 150, 255)   },  // Doppler Sapphire
    { 421,  IM_COL32(30, 10, 40, 255),     IM_COL32(60, 20, 80, 255),    IM_COL32(20, 15, 50, 255)   },  // Doppler Black Pearl
    // Gamma Doppler
    { 568,  IM_COL32(10, 80, 30, 255),     IM_COL32(20, 140, 50, 255),   IM_COL32(10, 60, 40, 255)   },  // Gamma Doppler P1
    { 569,  IM_COL32(30, 180, 80, 255),    IM_COL32(60, 220, 100, 255),  IM_COL32(20, 140, 60, 255)  },  // Gamma Doppler P2
    { 570,  IM_COL32(20, 60, 40, 255),     IM_COL32(40, 100, 70, 255),   IM_COL32(15, 50, 30, 255)   },  // Gamma Doppler P3
    { 571,  IM_COL32(10, 120, 60, 255),    IM_COL32(30, 200, 80, 255),   IM_COL32(10, 100, 50, 255)  },  // Gamma Doppler P4
    { 572,  IM_COL32(0, 200, 50, 255),     IM_COL32(0, 255, 80, 255),    IM_COL32(0, 180, 40, 255)   },  // Gamma Doppler Emerald
    { 1119, IM_COL32(10, 80, 30, 255),     IM_COL32(20, 140, 50, 255),   IM_COL32(10, 60, 40, 255)   },  // Gamma Doppler P1 (alt)
    // Classic skins
    { 38,   IM_COL32(255, 220, 50, 255),   IM_COL32(255, 120, 180, 255), IM_COL32(120, 50, 200, 255) },  // Fade
    { 59,   IM_COL32(180, 180, 200, 255),  IM_COL32(220, 40, 40, 255),   IM_COL32(200, 200, 220, 255)},  // Slaughter
    { 44,   IM_COL32(60, 80, 200, 255),    IM_COL32(200, 180, 50, 255),  IM_COL32(80, 100, 180, 255) },  // Case Hardened
    { 12,   IM_COL32(140, 10, 10, 255),    IM_COL32(180, 20, 20, 255),   IM_COL32(100, 5, 5, 255)    },  // Crimson Web
    { 409,  IM_COL32(220, 180, 30, 255),   IM_COL32(255, 210, 50, 255),  IM_COL32(200, 160, 20, 255) },  // Tiger Tooth
    { 413,  IM_COL32(200, 30, 30, 255),    IM_COL32(255, 220, 50, 255),  IM_COL32(30, 80, 200, 255)  },  // Marble Fade
    { 558,  IM_COL32(120, 80, 40, 255),    IM_COL32(180, 140, 60, 255),  IM_COL32(100, 70, 30, 255)  },  // Lore
    { 98,   IM_COL32(80, 20, 120, 255),    IM_COL32(120, 40, 180, 255),  IM_COL32(60, 15, 100, 255)  },  // Ultraviolet
    { 410,  IM_COL32(160, 160, 170, 255),  IM_COL32(200, 200, 210, 255), IM_COL32(140, 140, 150, 255)},  // Damascus Steel
    { 414,  IM_COL32(140, 80, 40, 255),    IM_COL32(180, 100, 50, 255),  IM_COL32(100, 60, 30, 255)  },  // Rust Coat
    { 43,   IM_COL32(20, 20, 30, 255),     IM_COL32(40, 40, 60, 255),    IM_COL32(15, 15, 25, 255)   },  // Night
    { 42,   IM_COL32(80, 100, 140, 255),   IM_COL32(120, 140, 180, 255), IM_COL32(60, 80, 120, 255)  },  // Blue Steel
    { 40,   IM_COL32(80, 80, 80, 255),     IM_COL32(120, 120, 120, 255), IM_COL32(60, 60, 60, 255)   },  // Urban Masked
    { 175,  IM_COL32(140, 100, 50, 255),   IM_COL32(180, 130, 60, 255),  IM_COL32(120, 80, 40, 255)  },  // Scorched
    { 5,    IM_COL32(40, 80, 30, 255),     IM_COL32(80, 120, 50, 255),   IM_COL32(30, 60, 20, 255)   },  // Forest DDPAT
    { 77,   IM_COL32(50, 80, 50, 255),     IM_COL32(80, 120, 70, 255),   IM_COL32(40, 60, 40, 255)   },  // Boreal Forest
    { 135,  IM_COL32(180, 170, 150, 255),  IM_COL32(200, 190, 170, 255), IM_COL32(160, 150, 130, 255)},  // Vanilla
    { 411,  IM_COL32(200, 40, 20, 255),    IM_COL32(180, 30, 15, 255),   IM_COL32(220, 50, 25, 255)  },  // Autotronic
};
static const int g_nKnifeGradientCount = sizeof(g_KnifeGradients) / sizeof(g_KnifeGradients[0]);

// Draw a 3-color horizontal gradient rectangle for knife skin preview
static void DrawKnifeSkinPreview(ImDrawList* pDrawList, ImVec2 pos, ImVec2 size, int nPaintKit)
{
    if (nPaintKit == 0) return;

    // Find gradient for this paint kit
    ImU32 colL = IM_COL32(80, 80, 80, 255);
    ImU32 colM = IM_COL32(120, 120, 120, 255);
    ImU32 colR = IM_COL32(80, 80, 80, 255);

    for (int i = 0; i < g_nKnifeGradientCount; i++)
    {
        if (g_KnifeGradients[i].m_nPaintKit == nPaintKit)
        {
            colL = g_KnifeGradients[i].m_colLeft;
            colM = g_KnifeGradients[i].m_colMid;
            colR = g_KnifeGradients[i].m_colRight;
            break;
        }
    }

    // Draw 2-part gradient: left->mid, mid->right
    float halfW = size.x * 0.5f;
    ImVec2 midPos = ImVec2(pos.x + halfW, pos.y);

    pDrawList->AddRectFilledMultiColor(
        pos, ImVec2(midPos.x, pos.y + size.y),
        colL, colM, colM, colL
    );
    pDrawList->AddRectFilledMultiColor(
        midPos, ImVec2(pos.x + size.x, pos.y + size.y),
        colM, colR, colR, colM
    );

    // Border
    pDrawList->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(200, 200, 200, 100), 3.f);
}

// Draw a small inline gradient swatch (for dropdown items)
static void DrawKnifeSwatchInline(ImDrawList* pDrawList, ImVec2 pos, float height, int nPaintKit)
{
    float swatchW = 40.f;
    float swatchH = height - 4.f;
    ImVec2 swatchPos = ImVec2(pos.x, pos.y + 2.f);
    DrawKnifeSkinPreview(pDrawList, swatchPos, ImVec2(swatchW, swatchH), nPaintKit);
}

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
    ImGui::Text("  [QULFLANGAN]  %s  -  %s daraja kerak",
        szFeatureName,
        eRequired == ETier::MID ? "MID" : "PRO");
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, C(120, 120, 140));
    ImGui::Text("  Yangilash shifthub.uz orqali");
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
        snprintf(buf, sizeof(buf), "[ Tugmani bosing... ]##%s", id);
        ImGui::Button(buf, ImVec2(140.f, 0.f));
        ImGui::PopStyleColor(3);

        // Scan all keys (BUT wait 0.2s before accepting, so we don't catch the left-click used to press this button)
        if (s_flListenTimer < 4.8f) // 5.0f - 0.2f
        {
            for (int i = 1; i < 256; i++)
            {
                if (GetAsyncKeyState(i) & 0x8000)
                {
                    if (i == VK_ESCAPE)
                    {
                        // ESC always cancels
                        s_pListening = nullptr;
                    }
                    else
                    {
                        key = i;
                        s_pListening = nullptr;
                    }
                    break;
                }
            }
        }
    }
    else
    {
        // Show current key name + Bind button
        char btnLabel[64];
        snprintf(btnLabel, sizeof(btnLabel), "[ %-8s ] O'rnatish##%s", GetKeyName(key), id);

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
    ImGui::Text("(ESC=bekor qilish)");
    ImGui::PopStyleColor();
}

// -----------------------------------------------------------------------
// Initialize fonts
// -----------------------------------------------------------------------
void Gui::Initialize(unsigned int uFontFlags)
{
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; // Disable imgui.ini generation

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

    // Auto-update: tekshirish (birinchi marta)
    static bool s_bUpdateChecked = false;
    if (!s_bUpdateChecked)
    {
        s_bUpdateChecked = true;
        g_Updater.CheckForUpdate();
    }

    // ================================================================
    // Title bar text:  SHIFTHUB  |  vX.X  |  [TIER]  |  User
    // ================================================================
    char szTitle[128];
    snprintf(szTitle, sizeof(szTitle),
        " SHIFTHUB.UZ  |  v%s  |  [ %s ]  |  %s",
        SHIFTHUB_VERSION,
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
        ImGui::Text("  Daraja: %s   |   %s   |   shifthub.uz",
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
            if (ImGui::BeginTabItem(X("[ VIZUAL ]")))
            {
                ImGui::Spacing();
                SectionTitle("Devor orti ko'rish (WH)");

                ImGui::Checkbox(X("Devordan ko'rishni yoqish"), &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bEnableVisuals));

                if (CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bEnableVisuals))
                {
                    ImGui::Spacing();
                    {
                        auto& vecMod = Config::Get<std::vector<bool>>(g_Variables.m_PlayerVisuals.m_vecVisualsModifiers);
                        bool bIgnoreTeam = vecMod[VISUALS_IGNORE_TEAMMATES];
                        bool bOnlyVis    = vecMod[VISUALS_ONLY_WHEN_VISIBLE];
                        if (ImGui::Checkbox(X("Jamoani e'tiborsiz"), &bIgnoreTeam)) vecMod[VISUALS_IGNORE_TEAMMATES] = bIgnoreTeam;
                        ImGui::SameLine(220.f);
                        if (ImGui::Checkbox(X("Faqat ko'ringanlar"), &bOnlyVis))        vecMod[VISUALS_ONLY_WHEN_VISIBLE] = bOnlyVis;
                    }

                    ImGui::Spacing();
                    SectionTitle("Quti");

                    ImGui::Checkbox(X("Quti chizish"), &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawBox));
                    if (CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawBox))
                    {
                        static const char* boxTypes[] = { "2D Quti", "Burchak", "Ikkisi" };
                        ImGui::SetNextItemWidth(160.f);
                        ImGui::Combo(X("Quti turi"), &CONFIG_GET(int, g_Variables.m_PlayerVisuals.m_iBoxType), boxTypes, 3);
                        ImGui::Checkbox(X("Quti chegarasi"), &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawBoxOutline));
                    }

                    ImGui::Spacing();
                    SectionTitle("Qo'shimcha ma'lumotlar");

                    ImGui::Checkbox(X("Jon paneli"),  &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawHealthBar));
                    ImGui::SameLine(220.f);
                    ImGui::Checkbox(X("Ism"), &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawName));

                    ImGui::Checkbox(X("Qurol nomi"), &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawWeapon));
                    ImGui::SameLine(220.f);
                    ImGui::Checkbox(X("Masofa"),    &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawDistance));

                    ImGui::Checkbox(X("Skelet"),    &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawSkeleton));
                    ImGui::SameLine(220.f);
                    ImGui::Checkbox(X("Bosh nuqta"),    &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawHeadDot));

                    ImGui::Checkbox(X("Tana bo'yash"), &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawFilledBody));
                    ImGui::SameLine(220.f);
                    ImGui::Checkbox(X("Chiziqlar"),  &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawSnaplines));

                    ImGui::Checkbox(X("Bomba ogohlantirish"), &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawHasC4));
                    ImGui::SameLine(220.f);
                    ImGui::Checkbox(X("Ekrandan tashqari belgilar"), &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawOffScreen));

                    ImGui::Spacing();
                    SectionTitle("Yerdagi Qurollar (Loot)");
                    ImGui::Checkbox(X("Loot ESP yoqish"), &CONFIG_GET(bool, g_Variables.m_ESP.m_bDroppedWeapons));
                    if (CONFIG_GET(bool, g_Variables.m_ESP.m_bDroppedWeapons))
                    {
                        ImGui::SetNextItemWidth(200.f);
                        ImGui::SliderFloat(X("Loot Masofasi"), &CONFIG_GET(float, g_Variables.m_ESP.m_flWeaponDistance), 5.f, 200.f, "%.0f metragacha");
                    }

                ImGui::Spacing();
                SectionTitle("Tezkor tugma");

                KeyBind("ESP yoqish/o'chirish:", CONFIG_GET(int, g_Variables.m_Hotkeys.m_iESPToggleKey), "espkey");

                ImGui::Spacing();
                SectionTitle("Ranglar");

                    ColorEdit4Shim(X("Dushman (ko'ringan)"),  CONFIG_GET(Color, g_Variables.m_PlayerVisuals.m_colEnemyVisible));
                    ColorEdit4Shim(X("Dushman (yashirin)"), CONFIG_GET(Color, g_Variables.m_PlayerVisuals.m_colEnemyOccluded));
                    ColorEdit4Shim(X("Jamoadosh"),       CONFIG_GET(Color, g_Variables.m_PlayerVisuals.m_colTeammate));
                }

                ImGui::Spacing();
                SectionTitle("Dunyo");
                ImGui::Checkbox(X("Tungi rejim"), &CONFIG_GET(bool, g_Variables.m_World.m_bNightMode));
                if (CONFIG_GET(bool, g_Variables.m_World.m_bNightMode))
                {
                    ImGui::SetNextItemWidth(200.f);
                    ImGui::SliderFloat(X("Tungi qorong'ulik"), &CONFIG_GET(float, g_Variables.m_World.m_flNightModeValue), 0.01f, 1.0f, "%.2f");
                }

                ImGui::Checkbox(X("O'yin kamerasini uzoqlashtirish (FOV)"), &CONFIG_GET(bool, g_Variables.m_World.m_bFOVChanger));
                if (CONFIG_GET(bool, g_Variables.m_World.m_bFOVChanger))
                {
                    ImGui::SetNextItemWidth(200.f);
                    ImGui::SliderInt(X("Kamera FOV"), &CONFIG_GET(int, g_Variables.m_World.m_iFOV), 40, 150, "%d deg");
                }

                ImGui::Spacing();
                ImGui::Spacing();
                SectionTitle("Odamni yoritish (Glow)");
                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                ImGui::Text("  Dushman yoki sizni fosfordek yoritib turadi");
                ImGui::PopStyleColor();

                ImGui::Checkbox(X("Yoritishni yoqish"), &CONFIG_GET(bool, g_Variables.m_PlayerGlow.m_bEnableGlow));

                if (CONFIG_GET(bool, g_Variables.m_PlayerGlow.m_bEnableGlow))
                {
                    ImGui::Checkbox(X("Glow bilan Jon va Qurolni ko'rsatish"), &CONFIG_GET(bool, g_Variables.m_PlayerGlow.m_bGlowInfo));
                    ImGui::Checkbox(X("Faqat dushmanlarni yoritish##glow"), &CONFIG_GET(bool, g_Variables.m_PlayerGlow.m_bGlowEnemyOnly));

                    static const char* glowTypes[] = { "O'chirilgan", "Oddiy", "Pulsating", "Tashqi Kontur" };
                    ImGui::SetNextItemWidth(160.f);
                    ImGui::Combo(X("Yoritish turi"), &CONFIG_GET(int, g_Variables.m_PlayerGlow.m_iGlowType), glowTypes, 4);

                    ColorEdit4Shim(X("Dushman rangi##gcol"), CONFIG_GET(Color, g_Variables.m_PlayerGlow.m_colGlowEnemy));
                    if (!CONFIG_GET(bool, g_Variables.m_PlayerGlow.m_bGlowEnemyOnly))
                        ColorEdit4Shim(X("Jamoadosh rangi##gcol2"), CONFIG_GET(Color, g_Variables.m_PlayerGlow.m_colGlowTeam));
                }

                ImGui::EndTabItem();
            }

            // ============================================================
            // MOVEMENT  (MID+)
            // ============================================================
            if (ImGui::BeginTabItem(X("[ HARAKAT ]")))
            {
                ImGui::Spacing();

                if (g_License.HasFeature(ETier::MID))
                {
                    SectionTitle("Bunny Hop");

                    ImGui::Checkbox(X("Bhop yoqish"), &CONFIG_GET(bool, g_Variables.m_Bhop.m_bEnableBhop));

                    if (CONFIG_GET(bool, g_Variables.m_Bhop.m_bEnableBhop))
                    {
                        ImGui::Checkbox(X("Auto-Strafer (Avtomatik sakrash)"), &CONFIG_GET(bool, g_Variables.m_Bhop.m_bEnableAutoStrafe));
                        ImGui::Spacing();
                        ImGui::PushStyleColor(ImGuiCol_Text, C(255, 200, 50));
                        ImGui::Text("  Space (32) ishlatmang!");
                        ImGui::PopStyleColor();
                        KeyBind("Ushlab turish:", CONFIG_GET(int, g_Variables.m_Bhop.m_iBhopKey), "bhopkey");
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
            if (ImGui::BeginTabItem(X("[ JANG ]")))
            {
                ImGui::Spacing();

                // --- Triggerbot (MID+) ---
                SectionTitle("Triggerbot");

                if (g_License.HasFeature(ETier::MID))
                {
                    ImGui::Checkbox(X("Triggerbot yoqish"), &CONFIG_GET(bool, g_Variables.m_TriggerBot.m_bEnableTriggerbot));
                    ImGui::Spacing();

                    bool& bAutoShoot = CONFIG_GET(bool, g_Variables.m_TriggerBot.m_bAutoShoot);
                    ImGui::Checkbox(X("Avto otish (tugmasiz)"), &bAutoShoot);

                    if (!bAutoShoot)
                    {
                        KeyBind("Ushlab turish:", CONFIG_GET(int, g_Variables.m_TriggerBot.m_iTriggerKey), "trigkey");
                        ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                        ImGui::Text("  OTISH kerak bo'lganda shu tugmani bosing");
                        ImGui::PopStyleColor();
                    }
                    else
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, C(0, 200, 65));
                        ImGui::Text("  [+] Avtomatik otadi (dushman nishonga tushganda)");
                        ImGui::PopStyleColor();
                    }

                    ImGui::SetNextItemWidth(200.f);
                    ImGui::SliderFloat(X("Otish kechikishi"), &CONFIG_GET(float, g_Variables.m_TriggerBot.m_flShotDelay), 0.f, 300.f, "%.0f ms");

                    ImGui::Checkbox(X("Jamoani e'tiborsiz##trig"), &CONFIG_GET(bool, g_Variables.m_TriggerBot.m_bIgnoreTeammates));
                    ImGui::Checkbox(X("Faqat ko'ringanlar (devordan emas)"), &CONFIG_GET(bool, g_Variables.m_TriggerBot.m_bOnlyVisible));

                    ImGui::Spacing();
                    SectionTitle("Anti-Ban Sozlamalari (V2.0)");

                    ImGui::SetNextItemWidth(200.f);
                    ImGui::SliderFloat(X("Hitchance##trig"), &CONFIG_GET(float, g_Variables.m_TriggerBot.m_flHitchance), 0.f, 100.f, "%.0f%%");
                    ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                    ImGui::Text("  80%% = har 5 ta dushmanfan 4 tasiga otadi (20%% miss)");
                    ImGui::PopStyleColor();

                    ImGui::Spacing();
                    ImGui::SetNextItemWidth(120.f);
                    ImGui::SliderInt(X("Min Burst##trig"), &CONFIG_GET(int, g_Variables.m_TriggerBot.m_iMinBurst), 1, 5, "%d o'q");
                    ImGui::SetNextItemWidth(120.f);
                    ImGui::SliderInt(X("Max Burst##trig"), &CONFIG_GET(int, g_Variables.m_TriggerBot.m_iMaxBurst), 1, 5, "%d o'q");
                    ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                    ImGui::Text("  Bir marta bosishda 1-3 ta o'q otadi (odam-o'xshash)");
                    ImGui::PopStyleColor();
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
                    ImGui::Checkbox(X("Aimbot yoqish"), &CONFIG_GET(bool, g_Variables.m_AimBot.m_bEnableAimbot));
                    ImGui::Spacing();

                    KeyBind("Nishon tugmasi:", CONFIG_GET(int, g_Variables.m_AimBot.m_iAimKey), "aimkey");

                    ImGui::SetNextItemWidth(200.f);
                    ImGui::SliderFloat(X("FOV"), &CONFIG_GET(float, g_Variables.m_AimBot.m_flFOV), 0.5f, 30.f, "%.1f deg");

                    ImGui::SetNextItemWidth(200.f);
                    ImGui::SliderFloat(X("Silliqlik"), &CONFIG_GET(float, g_Variables.m_AimBot.m_flSmooth), 1.f, 20.f, "%.1f");
                    ImGui::PushStyleColor(ImGuiCol_Text, C(120, 120, 140));
                    ImGui::SameLine(); ImGui::Text("(1=tez, 10=silliq)");
                    ImGui::PopStyleColor();

                    static const char* hitboxNames[] = { "Bosh", "Bo'yin", "Ko'krak" };
                    ImGui::SetNextItemWidth(140.f);
                    ImGui::Combo(X("Nishon joyi"), &CONFIG_GET(int, g_Variables.m_AimBot.m_iHitbox), hitboxNames, 3);

                    ImGui::Checkbox(X("Jamoani e'tiborsiz##aim"), &CONFIG_GET(bool, g_Variables.m_AimBot.m_bIgnoreTeammates));
                    ImGui::Checkbox(X("FOV doirasini chizish"),       &CONFIG_GET(bool, g_Variables.m_AimBot.m_bDrawFOV));

                    ImGui::Spacing();
                    SectionTitle("Aimbot Rejimi (V2.0)");

                    static const char* aimModes[] = { "Klassik (Eski — Tez)", "Xavfsiz (V2.0 — Anti-Ban)" };
                    ImGui::SetNextItemWidth(260.f);
                    ImGui::Combo(X("Rejim##aimmode"), &CONFIG_GET(int, g_Variables.m_AimBot.m_iAimMode), aimModes, 2);

                    int iCurrentMode = CONFIG_GET(int, g_Variables.m_AimBot.m_iAimMode);

                    if (iCurrentMode == 0)
                    {
                        // Klassik rejim — info ko'rsatish
                        ImGui::Spacing();
                        ImGui::PushStyleColor(ImGuiCol_Text, C(255, 200, 50));
                        ImGui::Text("  [!] KLASSIK REJIM — Anti-ban himoyasi O'CHIRILGAN");
                        ImGui::PopStyleColor();
                        ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                        ImGui::Text("  Eski V1.0 aim — tez, lekin Overwatch'da ko'rinishi mumkin.");
                        ImGui::Text("  Delay yo'q, jitter yo'q, visibility check yo'q.");
                        ImGui::PopStyleColor();
                    }
                    else
                    {
                        // Xavfsiz rejim — barcha sozlamalar
                        ImGui::Spacing();
                        ImGui::PushStyleColor(ImGuiCol_Text, C(0, 220, 70));
                        ImGui::Text("  [+] XAVFSIZ REJIM — Anti-ban himoyasi YOQILGAN");
                        ImGui::PopStyleColor();

                        ImGui::Spacing();
                        ImGui::Checkbox(X("Devordan aim QILMASIN (Visibility Check)"), &CONFIG_GET(bool, g_Variables.m_AimBot.m_bVisibilityCheck));
                        ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                        ImGui::Text("  Faqat ko'rinadigan dushmanlarga aim qiladi");
                        ImGui::PopStyleColor();

                        ImGui::Spacing();
                        ImGui::SetNextItemWidth(200.f);
                        ImGui::SliderFloat(X("Min reaktsiya (ms)##aim"), &CONFIG_GET(float, g_Variables.m_AimBot.m_flReactionTimeMin), 0.f, 500.f, "%.0f ms");
                        ImGui::SetNextItemWidth(200.f);
                        ImGui::SliderFloat(X("Max reaktsiya (ms)##aim"), &CONFIG_GET(float, g_Variables.m_AimBot.m_flReactionTimeMax), 0.f, 500.f, "%.0f ms");
                        ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                        ImGui::Text("  Yangi targetga aim olishdan oldin kutish (odam reaktsiyasi)");
                        ImGui::PopStyleColor();

                        ImGui::Spacing();
                        ImGui::SetNextItemWidth(200.f);
                        ImGui::SliderFloat(X("Maks aim vaqt (ms)##aim"), &CONFIG_GET(float, g_Variables.m_AimBot.m_flMaxAimTime), 500.f, 10000.f, "%.0f ms");
                        ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                        ImGui::Text("  Bir targetga uzoq aim qilsa — to'xtaydi");
                        ImGui::PopStyleColor();

                        ImGui::Spacing();
                        ImGui::SetNextItemWidth(200.f);
                        ImGui::SliderFloat(X("Aim xatolik (px)##aim"), &CONFIG_GET(float, g_Variables.m_AimBot.m_flAimJitter), 0.f, 10.f, "%.1f px");
                        ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                        ImGui::Text("  Tasodifiy xatolik — 100%% aniq emas (odam o'xshash)");
                        ImGui::PopStyleColor();

                        ImGui::Spacing();
                        ImGui::SetNextItemWidth(200.f);
                        ImGui::SliderFloat(X("Kill kutish (ms)##aim"), &CONFIG_GET(float, g_Variables.m_AimBot.m_flKillDelay), 0.f, 1500.f, "%.0f ms");
                        ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                        ImGui::Text("  Dushman o'lganda keyingisiga o'tishdan oldin pauza");
                        ImGui::PopStyleColor();
                    }

                    ImGui::Spacing();
                    SectionTitle("Recoil Control (RCS) - Otkacha");
                    ImGui::Checkbox(X("RCS yoqish"), &CONFIG_GET(bool, g_Variables.m_RCS.m_bEnable));
                    if (CONFIG_GET(bool, g_Variables.m_RCS.m_bEnable))
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                        ImGui::Text("Aimbot yoqilgan payti ishlamaydi, mustaqil tortadi.");
                        ImGui::PopStyleColor();
                        ImGui::SetNextItemWidth(200.f);
                        ImGui::SliderFloat(X("RCS X (Yonga) kuch"), &CONFIG_GET(float, g_Variables.m_RCS.m_flScaleX), 0.f, 2.0f, "%.2f");
                        ImGui::SetNextItemWidth(200.f);
                        ImGui::SliderFloat(X("RCS Y (Pastga) kuch"), &CONFIG_GET(float, g_Variables.m_RCS.m_flScaleY), 0.f, 2.0f, "%.2f");
                    }
                }
                else
                {
                    LockedFeature("Aimbot", ETier::PRO);
                }

                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::EndTabItem();
            }

            // ============================================================
            // RADAR  (all tiers)
            // ============================================================
            if (ImGui::BeginTabItem(X("[ RADAR ]")))
            {
                ImGui::Spacing();
                SectionTitle("2D Radar");

                ImGui::Checkbox(X("Radar yoqish"), &CONFIG_GET(bool, g_Variables.m_Radar.m_bEnableRadar));

                if (CONFIG_GET(bool, g_Variables.m_Radar.m_bEnableRadar))
                {
                    ImGui::Spacing();
                    ImGui::SetNextItemWidth(180.f);
                    ImGui::SliderFloat(X("Radar kattaligi"),   &CONFIG_GET(float, g_Variables.m_Radar.m_flRadarSize),  100.f, 400.f, "%.0f px");
                    ImGui::SetNextItemWidth(180.f);
                    ImGui::SliderFloat(X("Radar masofasi"),  &CONFIG_GET(float, g_Variables.m_Radar.m_flRadarRange), 500.f, 5000.f, "%.0f u");
                    ImGui::SetNextItemWidth(180.f);
                    ImGui::SliderFloat(X("X pozitsiya"),   &CONFIG_GET(float, g_Variables.m_Radar.m_flRadarX),    0.f, 1920.f, "%.0f");
                    ImGui::SetNextItemWidth(180.f);
                    ImGui::SliderFloat(X("Y pozitsiya"),   &CONFIG_GET(float, g_Variables.m_Radar.m_flRadarY),    0.f, 1080.f, "%.0f");
                    ImGui::Checkbox(X("Qarash bilan aylantirish"), &CONFIG_GET(bool, g_Variables.m_Radar.m_bRadarRotate));
                }

                ImGui::Spacing();
                ImGui::Spacing();
                SectionTitle("O'yin radari (In-Game Hack)");
                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                ImGui::Text("  Dushmanlarni o'yinning chap tepasidagi asil radariga ochib beradi.");
                ImGui::PopStyleColor();

                ImGui::Checkbox(X("O'yin radarida ko'rsatish"), &CONFIG_GET(bool, g_Variables.m_Radar.m_bInGameRadar));

                ImGui::Spacing();
                ImGui::Spacing();
                SectionTitle("Ovozli Radar (Sonar ESP)");
                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                ImGui::Text("  Dushmanga qaraganingizda devor ortidan ovoz (Piip) chiqaradi.");
                ImGui::PopStyleColor();

                ImGui::Checkbox(X("Ovozli radarni yoqish"), &CONFIG_GET(bool, g_Variables.m_Misc.m_bEnableSonar));
                if (CONFIG_GET(bool, g_Variables.m_Misc.m_bEnableSonar))
                {
                    ImGui::SetNextItemWidth(180.f);
                    ImGui::SliderFloat(X("Sezuvchanlik radiusi (FOV)"), &CONFIG_GET(float, g_Variables.m_Misc.m_flSonarFOV), 1.f, 20.f, "%.1f deg");
                }

                ImGui::EndTabItem();
            }

            // ============================================================
            // GRENADE HELPER
            // ============================================================
            if (ImGui::BeginTabItem(X("[ GRANATA ]")))
            {
                ImGui::Spacing();
                SectionTitle("Granata Yordamchi (Grenade Helper)");

                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                ImGui::Text("  Xaritadagi ma'lum joylardan smoke/flash/molotov tashlash yo'lini ko'rsatadi.");
                ImGui::Text("  Pozitsiyaga yaqinlashganingizda nishon va yo'nalish ko'rsatiladi.");
                ImGui::PopStyleColor();
                ImGui::Spacing();

                ImGui::Checkbox(X("Granata yordamchini yoqish"), &CONFIG_GET(bool, g_Variables.m_GrenadeHelper.m_bEnable));

                if (CONFIG_GET(bool, g_Variables.m_GrenadeHelper.m_bEnable))
                {
                    ImGui::Spacing();
                    ImGui::Checkbox(X("Barcha joylarni xaritada ko'rsatish"), &CONFIG_GET(bool, g_Variables.m_GrenadeHelper.m_bShowAll));

                    if (CONFIG_GET(bool, g_Variables.m_GrenadeHelper.m_bShowAll))
                    {
                        ImGui::SetNextItemWidth(200.f);
                        ImGui::SliderFloat(X("Ko'rsatish masofasi"), &CONFIG_GET(float, g_Variables.m_GrenadeHelper.m_flMaxDistance), 500.f, 5000.f, "%.0f unit");
                    }

                    ImGui::Spacing();
                    SectionTitle("Granata turlari filtri");

                    ImGui::Checkbox(X("Smoke"),   &CONFIG_GET(bool, g_Variables.m_GrenadeHelper.m_bShowSmoke));
                    ImGui::SameLine(150.f);
                    ImGui::Checkbox(X("Flash"),   &CONFIG_GET(bool, g_Variables.m_GrenadeHelper.m_bShowFlash));

                    ImGui::Checkbox(X("Molotov"), &CONFIG_GET(bool, g_Variables.m_GrenadeHelper.m_bShowMolotov));
                    ImGui::SameLine(150.f);
                    ImGui::Checkbox(X("HE"),      &CONFIG_GET(bool, g_Variables.m_GrenadeHelper.m_bShowHE));

                    ImGui::Spacing();
                    ImGui::Spacing();
                    SectionTitle("Mavjud xaritalar");

                    // Show available maps and lineup counts
                    for (int i = 0; i < GrenadeHelper::g_nMapCount; i++)
                    {
                        const GrenadeMap_t& map = GrenadeHelper::g_vecMaps[i];
                        
                        // Count by type
                        int nSmoke = 0, nFlash = 0, nMolly = 0, nHE = 0;
                        for (int j = 0; j < map.m_nLineupCount; j++)
                        {
                            switch (map.m_pLineups[j].m_eType)
                            {
                            case GRENADE_SMOKE:   nSmoke++; break;
                            case GRENADE_FLASH:   nFlash++; break;
                            case GRENADE_MOLOTOV: nMolly++; break;
                            case GRENADE_HE:      nHE++;    break;
                            }
                        }

                        ImGui::PushStyleColor(ImGuiCol_Text, C(0, 220, 70));
                        ImGui::Text("  %s", map.m_szMapName);
                        ImGui::PopStyleColor();
                        ImGui::SameLine(140.f);
                        ImGui::PushStyleColor(ImGuiCol_Text, C(150, 170, 160));
                        ImGui::Text("%d lineup  |  S:%d  F:%d  M:%d  HE:%d",
                            map.m_nLineupCount, nSmoke, nFlash, nMolly, nHE);
                        ImGui::PopStyleColor();
                    }

                    ImGui::Spacing();
                    ImGui::PushStyleColor(ImGuiCol_Text, C(255, 200, 50));
                    ImGui::Text("  Qo'shimcha xaritalar keyingi yangilanishda qo'shiladi!");
                    ImGui::PopStyleColor();
                }

                ImGui::EndTabItem();
            }

            // ============================================================
            // INVENTORY  (Skin Changer)
            // ============================================================
            if (ImGui::BeginTabItem(X("[ INVENTAR ]")))
            {
                ImGui::Spacing();
                SectionTitle("Skin o'zgartirish");

                ImGui::Checkbox(X("Skin o'zgartirish yoqish"), &SkinChanger::m_bEnabled);
                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                ImGui::Text("  Qo'ldagi qurollar skinini o'zgartiradi");
                ImGui::PopStyleColor();

                if (SkinChanger::m_bEnabled)
                {
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    // ============================================================
                    // KNIFE SKINS — Maxsus bo'lim (V2.0)
                    // ============================================================
                    ImGui::PushStyleColor(ImGuiCol_Text, C(255, 200, 50));
                    ImGui::Text("  %s  PICHOQ SKINLARI", "\xF0\x9F\x94\xAA");
                    ImGui::PopStyleColor();
                    ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                    ImGui::Text("  Pichoqingiz uchun skin tanlang (Doppler, Fade, Tiger Tooth...)");
                    ImGui::PopStyleColor();
                    ImGui::Spacing();

                    {
                        WeaponSkinConfig_t& knifeCfg = SkinChanger::GetWeaponConfig(WEAPON_KNIFE_CT);

                        // Knife skin dropdown
                        const char* szCurrentKnifeSkin = "Default (o'zgarishsiz)";
                        for (int j = 0; j < SkinChanger::g_nKnifeSkinCount; j++)
                        {
                            if (SkinChanger::g_KnifeSkins[j].m_nPaintKit == knifeCfg.m_nPaintKit)
                            {
                                szCurrentKnifeSkin = SkinChanger::g_KnifeSkins[j].m_szName;
                                break;
                            }
                        }

                        // === KATTA PREVIEW — tanlangan skin gradienti ===
                        if (knifeCfg.m_nPaintKit != 0)
                        {
                            ImVec2 previewPos = ImGui::GetCursorScreenPos();
                            ImVec2 previewSize = ImVec2(300.f, 50.f);
                            ImDrawList* pDL = ImGui::GetWindowDrawList();

                            // Background
                            pDL->AddRectFilled(previewPos, ImVec2(previewPos.x + previewSize.x, previewPos.y + previewSize.y), IM_COL32(15, 15, 20, 255), 4.f);

                            // Gradient
                            ImVec2 gradPos = ImVec2(previewPos.x + 4.f, previewPos.y + 4.f);
                            ImVec2 gradSize = ImVec2(previewSize.x - 8.f, previewSize.y - 8.f);
                            DrawKnifeSkinPreview(pDL, gradPos, gradSize, knifeCfg.m_nPaintKit);

                            // Skin name overlay
                            char previewText[64];
                            snprintf(previewText, sizeof(previewText), "%s", szCurrentKnifeSkin);
                            ImVec2 textSize = ImGui::CalcTextSize(previewText);
                            ImVec2 textPos = ImVec2(
                                previewPos.x + (previewSize.x - textSize.x) * 0.5f,
                                previewPos.y + (previewSize.y - textSize.y) * 0.5f
                            );
                            // Shadow
                            pDL->AddText(ImVec2(textPos.x + 1.f, textPos.y + 1.f), IM_COL32(0, 0, 0, 200), previewText);
                            // Text
                            pDL->AddText(textPos, IM_COL32(255, 255, 255, 255), previewText);

                            ImGui::Dummy(previewSize);
                            ImGui::Spacing();
                        }

                        // === DROPDOWN — har bir skin yonida swatch ===
                        ImGui::SetNextItemWidth(300.f);
                        if (ImGui::BeginCombo(X("##KnifeSkinCombo"), szCurrentKnifeSkin))
                        {
                            for (int j = 0; j < SkinChanger::g_nKnifeSkinCount; j++)
                            {
                                bool bSelected = (SkinChanger::g_KnifeSkins[j].m_nPaintKit == knifeCfg.m_nPaintKit);
                                int nPK = SkinChanger::g_KnifeSkins[j].m_nPaintKit;

                                // Swatch + text label
                                if (nPK != 0)
                                {
                                    // Draw color swatch before text
                                    ImVec2 itemPos = ImGui::GetCursorScreenPos();
                                    float itemH = ImGui::GetTextLineHeight();
                                    DrawKnifeSwatchInline(ImGui::GetWindowDrawList(), itemPos, itemH, nPK);
                                }

                                // Label with indent for swatch space
                                char knifeLabel[96];
                                if (nPK == 0)
                                    snprintf(knifeLabel, sizeof(knifeLabel), "      Default (off)");
                                else
                                    snprintf(knifeLabel, sizeof(knifeLabel), "           %s", SkinChanger::g_KnifeSkins[j].m_szName);

                                if (ImGui::Selectable(knifeLabel, bSelected))
                                    knifeCfg.m_nPaintKit = nPK;

                                if (bSelected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }

                        // Knife wear
                        ImGui::SetNextItemWidth(200.f);
                        ImGui::SliderFloat(X("Sifat##knifewear"), &knifeCfg.m_flWear, 0.0001f, 1.0f, "%.4f");
                        ImGui::SameLine();
                        ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                        if (knifeCfg.m_flWear < 0.07f)       ImGui::Text("(Factory New)");
                        else if (knifeCfg.m_flWear < 0.15f)  ImGui::Text("(Minimal Wear)");
                        else if (knifeCfg.m_flWear < 0.38f)  ImGui::Text("(Field-Tested)");
                        else if (knifeCfg.m_flWear < 0.45f)  ImGui::Text("(Well-Worn)");
                        else                                  ImGui::Text("(Battle-Scarred)");
                        ImGui::PopStyleColor();

                        // Knife seed
                        ImGui::SetNextItemWidth(120.f);
                        ImGui::InputInt(X("Pattern##knifeseed"), &knifeCfg.m_nSeed);
                        if (knifeCfg.m_nSeed < 0) knifeCfg.m_nSeed = 0;
                        if (knifeCfg.m_nSeed > 1000) knifeCfg.m_nSeed = 1000;

                        // Knife StatTrak
                        bool bKnifeST = (knifeCfg.m_nStatTrak >= 0);
                        ImGui::SameLine(260.f);
                        if (ImGui::Checkbox(X("StatTrak##knifest"), &bKnifeST))
                            knifeCfg.m_nStatTrak = bKnifeST ? 0 : -1;
                        if (knifeCfg.m_nStatTrak >= 0)
                        {
                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(80.f);
                            ImGui::InputInt(X("##KnifeSTVal"), &knifeCfg.m_nStatTrak);
                            if (knifeCfg.m_nStatTrak < 0) knifeCfg.m_nStatTrak = 0;
                        }

                        // Reset knife
                        ImGui::PushStyleColor(ImGuiCol_Button,        C(120, 30, 30));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C(180, 50, 50));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  C(220, 60, 60));
                        if (ImGui::Button(X("Pichoq Reset##knifereset"), ImVec2(130.f, 0.f)))
                        {
                            knifeCfg.m_nPaintKit = 0;
                            knifeCfg.m_flWear    = 0.0001f;
                            knifeCfg.m_nSeed     = 0;
                            knifeCfg.m_nStatTrak = -1;
                        }
                        ImGui::PopStyleColor(3);

                        if (knifeCfg.m_nPaintKit != 0)
                        {
                            ImGui::SameLine();
                            ImGui::PushStyleColor(ImGuiCol_Text, C(0, 220, 70));
                            ImGui::Text("  Aktiv: %s", szCurrentKnifeSkin);
                            ImGui::PopStyleColor();
                        }
                    }

                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    // ============================================================
                    // QUROL SKINLARI
                    // ============================================================
                    SectionTitle("Qurol skinlari");

                    static int nSelectedWeapon = 0;

                    ImGui::BeginColumns(X("##SkinCols"), 2, ImGuiColumnsFlags_NoResize);
                    ImGui::SetColumnWidth(0, 180.f);
                    {
                        // === LEFT: Weapon list ===
                        ImGui::PushStyleColor(ImGuiCol_Text, C(0, 220, 70));
                        ImGui::TextUnformatted("Qurollar");
                        ImGui::PopStyleColor();
                        ImGui::Spacing();

                        if (ImGui::BeginListBox(X("##WeaponList"), ImVec2(-1, 280.f)))
                        {
                            // Knife ni qurollar listidan chiqaramiz (yuqorida alohida bo'limi bor)
                            for (int i = 0; i < SkinChanger::g_nWeaponCategoryCount; i++)
                            {
                                WeaponCategory_t& cat = SkinChanger::g_vecWeaponCategories[i];

                                // Knife'ni skip qilish — yuqorida alohida bo'limda
                                if (cat.m_nDefIndex == WEAPON_KNIFE_CT) continue;

                                WeaponSkinConfig_t& cfg = SkinChanger::GetWeaponConfig(cat.m_nDefIndex);

                                // Mark weapons that have a skin selected
                                char label[64];
                                if (cfg.m_nPaintKit != 0)
                                    snprintf(label, sizeof(label), "[*] %s", cat.m_szName);
                                else
                                    snprintf(label, sizeof(label), "    %s", cat.m_szName);

                                if (ImGui::Selectable(label, i == nSelectedWeapon))
                                    nSelectedWeapon = i;
                            }
                            ImGui::EndListBox();
                        }
                    }
                    ImGui::NextColumn();
                    {
                        // === RIGHT: Skin selector ===
                        if (nSelectedWeapon >= 0 && nSelectedWeapon < SkinChanger::g_nWeaponCategoryCount)
                        {
                            WeaponCategory_t& cat = SkinChanger::g_vecWeaponCategories[nSelectedWeapon];
                            WeaponSkinConfig_t& cfg = SkinChanger::GetWeaponConfig(cat.m_nDefIndex);

                            ImGui::PushStyleColor(ImGuiCol_Text, C(0, 220, 70));
                            ImGui::Text("%s - Skinlar", cat.m_szName);
                            ImGui::PopStyleColor();
                            ImGui::Spacing();

                            // Skin dropdown
                            const char* szCurrentSkin = "Default";
                            for (int j = 0; j < cat.m_nSkinCount; j++)
                            {
                                if (cat.m_pSkins[j].m_nPaintKit == cfg.m_nPaintKit)
                                {
                                    szCurrentSkin = cat.m_pSkins[j].m_szName;
                                    break;
                                }
                            }

                            ImGui::SetNextItemWidth(-1);
                            if (ImGui::BeginCombo(X("##SkinCombo"), szCurrentSkin))
                            {
                                for (int j = 0; j < cat.m_nSkinCount; j++)
                                {
                                    bool bSelected = (cat.m_pSkins[j].m_nPaintKit == cfg.m_nPaintKit);
                                    char skinLabel[96];
                                    if (cat.m_pSkins[j].m_nPaintKit == 0)
                                        snprintf(skinLabel, sizeof(skinLabel), "Default (off)");
                                    else
                                        snprintf(skinLabel, sizeof(skinLabel), "[%d] %s", cat.m_pSkins[j].m_nPaintKit, cat.m_pSkins[j].m_szName);

                                    if (ImGui::Selectable(skinLabel, bSelected))
                                        cfg.m_nPaintKit = cat.m_pSkins[j].m_nPaintKit;

                                    if (bSelected)
                                        ImGui::SetItemDefaultFocus();
                                }
                                ImGui::EndCombo();
                            }

                            ImGui::Spacing();
                            ImGui::Spacing();

                            // Wear slider
                            SectionTitle("Sifat (Wear)");
                            ImGui::SetNextItemWidth(-1);
                            ImGui::SliderFloat(X("##Wear"), &cfg.m_flWear, 0.0001f, 1.0f, "%.4f");

                            ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                            if (cfg.m_flWear < 0.07f)       ImGui::Text("  Factory New");
                            else if (cfg.m_flWear < 0.15f)  ImGui::Text("  Minimal Wear");
                            else if (cfg.m_flWear < 0.38f)  ImGui::Text("  Field-Tested");
                            else if (cfg.m_flWear < 0.45f)  ImGui::Text("  Well-Worn");
                            else                             ImGui::Text("  Battle-Scarred");
                            ImGui::PopStyleColor();

                            ImGui::Spacing();

                            // Seed
                            SectionTitle("Pattern Seed");
                            ImGui::SetNextItemWidth(120.f);
                            ImGui::InputInt(X("##Seed"), &cfg.m_nSeed);
                            if (cfg.m_nSeed < 0) cfg.m_nSeed = 0;
                            if (cfg.m_nSeed > 1000) cfg.m_nSeed = 1000;

                            ImGui::Spacing();

                            // StatTrak
                            SectionTitle("StatTrak");
                            bool bStatTrak = (cfg.m_nStatTrak >= 0);
                            if (ImGui::Checkbox(X("Enable StatTrak##st"), &bStatTrak))
                                cfg.m_nStatTrak = bStatTrak ? 0 : -1;

                            if (cfg.m_nStatTrak >= 0)
                            {
                                ImGui::SetNextItemWidth(120.f);
                                ImGui::InputInt(X("##StatTrakVal"), &cfg.m_nStatTrak);
                                if (cfg.m_nStatTrak < 0) cfg.m_nStatTrak = 0;
                            }

                            ImGui::Spacing();
                            ImGui::Spacing();

                            // Reset button
                            ImGui::PushStyleColor(ImGuiCol_Button,        C(120, 30, 30));
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C(180, 50, 50));
                            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  C(220, 60, 60));
                            if (ImGui::Button(X("Reset##skinreset"), ImVec2(120.f, 0.f)))
                            {
                                cfg.m_nPaintKit = 0;
                                cfg.m_flWear    = 0.0001f;
                                cfg.m_nSeed     = 0;
                                cfg.m_nStatTrak = -1;
                            }
                            ImGui::PopStyleColor(3);

                            ImGui::SameLine();

                            // Reset ALL
                            if (ImGui::Button(X("Reset All##skinresetall"), ImVec2(120.f, 0.f)))
                                SkinChanger::m_mapWeaponConfigs.clear();
                        }
                    }
                    ImGui::EndColumns();
                }

                ImGui::EndTabItem();
            }

            // ============================================================
            // CONFIGS  (all tiers)
            // ============================================================
            if (ImGui::BeginTabItem(X("[ SOZLAMALAR ]")))
            {
                ImGui::Spacing();
                SectionTitle("Sozlamalar boshqaruvi");

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
                    ImGui::InputTextWithHint(X("##cfgfile"), X("config nomi..."), &strCfgName);

                    if (ImGui::Button(X("Yaratish"), ImVec2(120.f, 0.f)))
                    {
                        Config::Save(strCfgName);
                        strCfgName.clear();
                        Config::Refresh();
                    }
                    if (ImGui::Button(X("Yangilash"), ImVec2(120.f, 0.f))) Config::Refresh();

                    if (nSelected != -1)
                    {
                        ImGui::Spacing();
                        if (ImGui::Button(X("Saqlash"),   ImVec2(120.f, 0.f))) Config::Save(Config::m_vecFileNames.at(nSelected));
                        if (ImGui::Button(X("Yuklash"),   ImVec2(120.f, 0.f))) Config::Load(Config::m_vecFileNames.at(nSelected));
                        if (ImGui::Button(X("O'chirish"), ImVec2(120.f, 0.f)))
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
            if (ImGui::BeginTabItem(X("[ UMUMIY ]")))
            {
                ImGui::Spacing();
                SectionTitle("Asosiy");

                ImGui::Text("Menyu tugmasi:");
                ImGui::SetNextItemWidth(120.f);
                ImGui::InputInt(X("##menukey"), &CONFIG_GET(int, g_Variables.m_Gui.m_iMenuKey));

                ImGui::Spacing();
                ImGui::PushStyleColor(ImGuiCol_Text, C(255, 200, 50));
                ImGui::Text("  [DELETE] = Hammasini o'chirish/yoqish (Panic)");
                ImGui::Text("  [END]    = Dasturni yopish");
                ImGui::PopStyleColor();

                ImGui::Checkbox(X("Ekran yozishdan yashirish"),
                    &CONFIG_GET(bool, g_Variables.m_Gui.m_bExcludeFromDesktopCapture));

                ImGui::Spacing();
                ImGui::Spacing();
                SectionTitle("Qo'shimcha xususiyatlar");

                ImGui::Checkbox(X("Avtomatik Qabul Qilish (Auto-Accept)"), &CONFIG_GET(bool, g_Variables.m_Misc.m_bAutoAccept));
                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                ImGui::Text("  Match topilganda orqa fonda avtomatik tarzda markaziy tugmani bosadi.");
                ImGui::PopStyleColor();

                ImGui::Checkbox(X("Flash himoya"), &CONFIG_GET(bool, g_Variables.m_Misc.m_bAntiFlash));
                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                ImGui::Text("  Flash grenade effektini yo'q qiladi");
                ImGui::PopStyleColor();

                ImGui::Checkbox(X("C4 Timer + Damage Indicator"), &CONFIG_GET(bool, g_Variables.m_Misc.m_bC4Timer));
                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                ImGui::Text("  Bomba portlashigacha qolgan vaqt va ZARARNI ko'rsatadi");
                ImGui::PopStyleColor();

                ImGui::Checkbox(X("Watermark"), &CONFIG_GET(bool, g_Variables.m_Misc.m_bWatermark));
                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                ImGui::Text("  Ekranda cheat nomi va FPS ko'rsatib turadi");
                ImGui::PopStyleColor();

                ImGui::Checkbox(X("Granata Xavfi (Grenade Warning)"), &CONFIG_GET(bool, g_Variables.m_Misc.m_bGrenadeWarning));
                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                ImGui::Text("  Molotov, Smoke va HE traektoriyasi va radiusini chizadi");
                ImGui::PopStyleColor();

                ImGui::Checkbox(X("Sniper Crosshair (qoq markaz)"), &CONFIG_GET(bool, g_Variables.m_Misc.m_bSniperCrosshair));
                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                ImGui::Text("  Snayper (AWP/SSG08) orzuqilmasdan (scope ochiqmas) turganda chizadi");
                ImGui::PopStyleColor();

                ImGui::Checkbox(X("Hit Sound (Tegish ovozi)"), &CONFIG_GET(bool, g_Variables.m_Misc.m_bHitSound));
                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                ImGui::Text("  Dushmanga o'q tekkanda 'Tink' ovozini chiqarish");
                ImGui::PopStyleColor();

                ImGui::Checkbox(X("Kill Sound (O'ldirish ovozi)"), &CONFIG_GET(bool, g_Variables.m_Misc.m_bKillSound));
                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                ImGui::Text("  Dushman o'lganda alohida ovoz (kill_sound.wav)");
                ImGui::PopStyleColor();

                ImGui::SetNextItemWidth(200.f);
                ImGui::SliderFloat(X("Ovoz balandligi##sndvol"), &CONFIG_GET(float, g_Variables.m_Misc.m_flSoundVolume), 0.f, 100.f, "%.0f%%");

                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                ImGui::Text("  hit_sound.wav va kill_sound.wav fayllarini exe yoniga qo'ying");
                ImGui::PopStyleColor();

                // V2.0: Sound test buttons
                ImGui::PushStyleColor(ImGuiCol_Button,        C(20, 80, 40));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C(30, 120, 60));
                if (ImGui::Button(X("Test Hit##sndhit"), ImVec2(80.f, 0.f)))
                    PlaySoundA("C:\\Windows\\Media\\Windows Default.wav", NULL, SND_ASYNC | SND_FILENAME);
                ImGui::SameLine();
                if (ImGui::Button(X("Test Kill##sndkill"), ImVec2(80.f, 0.f)))
                    PlaySoundA("C:\\Windows\\Media\\Windows Default.wav", NULL, SND_ASYNC | SND_FILENAME);
                ImGui::PopStyleColor(2);

                ImGui::Checkbox(X("Tomosha qiluvchilar"), &CONFIG_GET(bool, g_Variables.m_SpectatorList.m_bEnableSpectatorList));
                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                ImGui::Text("  Sizni kim tomosha qilayotganini ko'rsatadi");
                ImGui::PopStyleColor();

                ImGui::Spacing();
                ImGui::Spacing();
                SectionTitle("Litsenziya ma'lumoti");

                ImGui::PushStyleColor(ImGuiCol_Text, g_License.GetTierColor());
                ImGui::Text("  Daraja:    %s", g_License.GetTierName());
                ImGui::Text("  Foydalanuvchi: %s", g_License.m_strUser.c_str());
                ImGui::Text("  Holat:     %s", g_License.m_strExpiry.c_str());
                ImGui::PopStyleColor();

                ImGui::Spacing();
                ImGui::PushStyleColor(ImGuiCol_Text, C(100, 120, 110));
                ImGui::Text("  Yangilash uchun key.txt faylini exe yoniga qo'ying.");
                ImGui::Text("  Kalit formati:  SH-XXXXXXXX-M  (MID)  yoki  SH-XXXXXXXX-P  (PRO)");
                ImGui::Text("  Kalit olish: shifthub.uz");
                ImGui::PopStyleColor();

                // ==========================================================
                // AUTO-UPDATE SECTION
                // ==========================================================
                ImGui::Spacing();
                ImGui::Spacing();
                SectionTitle("Dastur yangilash");

                ImGui::Text("  Joriy versiya: v%s", SHIFTHUB_VERSION);

                if (g_Updater.m_bUpdateAvailable)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, C(50, 255, 100));
                    ImGui::Text("  Yangi versiya: v%s", g_Updater.m_strLatestVersion.c_str());
                    ImGui::PopStyleColor();

                    if (!g_Updater.m_strChangelog.empty())
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, C(180, 180, 200));
                        ImGui::TextWrapped("  O'zgarishlar: %s", g_Updater.m_strChangelog.c_str());
                        ImGui::PopStyleColor();
                    }

                    ImGui::Spacing();

                    // Progress bar (yuklab olish jarayonida)
                    if (g_Updater.m_bDownloading)
                    {
                        ImGui::ProgressBar(g_Updater.m_flProgress, ImVec2(300.f, 20.f));
                        ImGui::PushStyleColor(ImGuiCol_Text, C(255, 200, 50));
                        ImGui::Text("  %s", g_Updater.m_strStatusText.c_str());
                        ImGui::PopStyleColor();
                    }
                    else if (g_Updater.m_bDownloadComplete)
                    {
                        // Yuklab olish tugadi — O'rnatish tugmasi
                        ImGui::PushStyleColor(ImGuiCol_Button,        C(20, 140, 40));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C(30, 180, 60));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  C(15, 100, 30));
                        if (ImGui::Button("  O'RNATISH VA QAYTA ISHGA TUSHIRISH  ", ImVec2(340.f, 35.f)))
                        {
                            g_Updater.ApplyUpdate();
                        }
                        ImGui::PopStyleColor(3);

                        ImGui::PushStyleColor(ImGuiCol_Text, C(50, 255, 100));
                        ImGui::Text("  Tayyor! Bosing va dastur yangilanadi.");
                        ImGui::PopStyleColor();
                    }
                    else if (g_Updater.m_bDownloadFailed)
                    {
                        // Xatolik
                        ImGui::PushStyleColor(ImGuiCol_Text, C(255, 60, 60));
                        ImGui::Text("  %s", g_Updater.m_strStatusText.c_str());
                        ImGui::PopStyleColor();

                        ImGui::PushStyleColor(ImGuiCol_Button,        C(140, 40, 20));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C(180, 60, 30));
                        if (ImGui::Button("  Qayta urinish  ", ImVec2(160.f, 28.f)))
                        {
                            g_Updater.StartDownload();
                        }
                        ImGui::PopStyleColor(2);
                    }
                    else
                    {
                        // YANGILASH tugmasi
                        ImGui::PushStyleColor(ImGuiCol_Button,        C(20, 100, 180));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C(30, 140, 220));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  C(15, 80, 140));
                        if (ImGui::Button("  YANGILASH  ", ImVec2(200.f, 35.f)))
                        {
                            g_Updater.StartDownload();
                        }
                        ImGui::PopStyleColor(3);
                    }
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, C(100, 100, 120));
                    ImGui::Text("  %s", g_Updater.m_strStatusText.c_str());
                    ImGui::PopStyleColor();
                }

                // Tekshirish tugmasi
                ImGui::Spacing();
                ImGui::PushStyleColor(ImGuiCol_Button,        C(60, 60, 80));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C(80, 80, 110));
                if (ImGui::Button("  Qayta tekshirish  ", ImVec2(160.f, 25.f)))
                {
                    g_Updater.Recheck();
                }
                ImGui::PopStyleColor(2);

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}
