#include "../Includes.h"
#include <json.hpp>
using json = nlohmann::json;

// =====================================================================
//  SHIFTHUB  ::  CYBERPUNK LOGIN / LOADER
// =====================================================================

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static bool s_bDrag = false;
static POINT s_ptDrag = {};

static LRESULT CALLBACK LoginWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return 0;
    switch (msg)
    {
    case WM_LBUTTONDOWN: s_bDrag = true; GetCursorPos(&s_ptDrag); SetCapture(hWnd); return 0;
    case WM_MOUSEMOVE:
        if (s_bDrag) {
            POINT pt; GetCursorPos(&pt); RECT rc; GetWindowRect(hWnd, &rc);
            MoveWindow(hWnd, rc.left + pt.x - s_ptDrag.x, rc.top + pt.y - s_ptDrag.y,
                rc.right - rc.left, rc.bottom - rc.top, TRUE);
            s_ptDrag = pt;
        } return 0;
    case WM_LBUTTONUP: s_bDrag = false; ReleaseCapture(); return 0;
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// Non-blocking CS2 check
static bool IsCS2Running()
{
    PROCESSENTRY32 pe = {}; pe.dwSize = sizeof(pe);
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return false;
    bool found = false;
    if (Process32First(snap, &pe)) {
        do { if (!strcmp(pe.szExeFile, "cs2.exe")) { found = true; break; } }
        while (Process32Next(snap, &pe));
    }
    CloseHandle(snap);
    return found;
}

// ===================================================================
static ImFont* LoginFont(ImGuiIO& io, const char* const* paths, int n, float size, ImFontConfig* cfg, const ImWchar* ranges)
{
    for (int i = 0; i < n; i++)
    {
        std::error_code ec;
        if (!std::filesystem::exists(paths[i], ec)) continue;
        if (ImFont* f = io.Fonts->AddFontFromFileTTF(paths[i], size, cfg, ranges))
            return f;
    }
    return io.Fonts->AddFontDefault();
}

bool LoginWindow::Create()
{
    if (m_bInitialized) return true;
    int wndW = 520, wndH = 620;
    int scrW = GetSystemMetrics(SM_CXSCREEN), scrH = GetSystemMetrics(SM_CYSCREEN);

    m_wc = {};
    m_wc.cbSize = sizeof(WNDCLASSEXW);
    m_wc.style = CS_CLASSDC;
    m_wc.lpfnWndProc = LoginWndProc;
    m_wc.hInstance = GetModuleHandleW(NULL);
    m_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    m_wc.lpszClassName = L"SH_Login_v2";
    RegisterClassExW(&m_wc);

    // Build wide title with version
    wchar_t wszTitle[64];
    swprintf_s(wszTitle, L"SHIFTHUB v%hs", SHIFTHUB_VERSION);
    m_hWnd = CreateWindowExW(0, m_wc.lpszClassName, wszTitle,
        WS_POPUP | WS_VISIBLE, (scrW - wndW) / 2, (scrH - wndH) / 2, wndW, wndH,
        NULL, NULL, m_wc.hInstance, NULL);
    if (!m_hWnd) return false;

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1; sd.BufferDesc.Width = wndW; sd.BufferDesc.Height = wndH;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; sd.BufferDesc.RefreshRate = { 60, 1 };
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; sd.OutputWindow = m_hWnd;
    sd.SampleDesc.Count = 1; sd.Windowed = TRUE; sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL lvl[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    D3D_FEATURE_LEVEL fl;
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
        lvl, 2, D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice, &fl, &m_pContext) != S_OK)
        return false;

    ID3D11Texture2D* bb = nullptr;
    m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&bb);
    if (bb) { m_pDevice->CreateRenderTargetView(bb, NULL, &m_pRTV); bb->Release(); }

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(m_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; // Disable imgui.ini generation

    static const char* const kBody[] = { "C:\\Windows\\Fonts\\segoeui.ttf",  "C:\\Windows\\Fonts\\Verdana.ttf" };
    static const char* const kBold[] = { "C:\\Windows\\Fonts\\segoeuib.ttf", "C:\\Windows\\Fonts\\Verdanab.ttf", "C:\\Windows\\Fonts\\segoeui.ttf" };
    static const char* const kMono[] = { "C:\\Windows\\Fonts\\consola.ttf",  "C:\\Windows\\Fonts\\cour.ttf" };

    const ImWchar* ranges = UI::GlyphRanges();

    ImFontConfig cfg = {};
    cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_LightHinting;
    ImFontConfig cfgB = {};
    cfgB.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags_Bold;

    Fonts::Default = LoginFont(io, kBody, IM_ARRAYSIZE(kBody), 16.f, &cfg,  ranges);
    Fonts::Small   = LoginFont(io, kBody, IM_ARRAYSIZE(kBody), 12.f, &cfg,  ranges);
    Fonts::Title   = LoginFont(io, kBold, IM_ARRAYSIZE(kBold), 34.f, &cfgB, ranges);
    Fonts::Mono    = LoginFont(io, kMono, IM_ARRAYSIZE(kMono), 13.f, &cfgB, ranges);

    ImGuiFreeType::BuildFontAtlas(io.Fonts, 0);

    m_bInitialized = true;
    return true;
}

// ===================================================================
//  Local drawing helpers
// ===================================================================
static void CenteredText(ImDrawList* dl, float W, float y, const char* txt, ImU32 col, ImFont* font = nullptr)
{
    if (font) ImGui::PushFont(font);
    ImVec2 ts = ImGui::CalcTextSize(txt);
    dl->AddText(ImVec2((W - ts.x) * 0.5f, y), col, txt);
    if (font) ImGui::PopFont();
}

// Animated background: grid + drifting data streaks + scanlines + vignette
static void DrawBackdrop(ImDrawList* dl, float W, float H, float flTime)
{
    dl->AddRectFilledMultiColor(ImVec2(0, 0), ImVec2(W, H),
        IM_COL32(8, 11, 18, 255), IM_COL32(8, 11, 18, 255),
        IM_COL32(4, 6, 11, 255),  IM_COL32(6, 5, 12, 255));

    // perspective-ish grid
    UI::Grid(dl, ImVec2(0, 0), ImVec2(W, H), IM_COL32(34, 226, 255, 8), 26.f);

    // drifting vertical data streaks
    for (int i = 0; i < 7; i++)
    {
        float x  = fmodf(37.f + (float)i * 71.f, W);
        float sp = 40.f + (float)((i * 37) % 60);
        float y  = fmodf(flTime * sp + (float)i * 90.f, H + 160.f) - 160.f;
        ImU32 c  = (i % 3 == 0) ? UI::COL_MAGENTA : UI::COL_CYAN;
        dl->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + 1.f, y + 150.f),
            UI::Fade(c, 0.f), UI::Fade(c, 0.f), UI::Fade(c, 0.22f), UI::Fade(c, 0.22f));
    }

    // scanlines + top/bottom neon frame
    UI::Scanlines(dl, ImVec2(0, 0), ImVec2(W, H), IM_COL32(0, 0, 0, 26), 3.f);

    dl->AddRectFilledMultiColor(ImVec2(0, 0), ImVec2(W, 3.f),
        UI::COL_CYAN, UI::COL_MAGENTA, UI::COL_MAGENTA, UI::COL_CYAN);

    dl->AddRect(ImVec2(1, 1), ImVec2(W - 1, H - 1), UI::Fade(UI::COL_CYAN, 0.35f), 0.f, 0, 1.f);
    UI::Brackets(dl, ImVec2(6, 6), ImVec2(W - 6, H - 6), UI::COL_CYAN, 22.f, 1.8f);
}

// ===================================================================
// ---------------------------------------------------------------------
// Login/FREE dan keyin tier ga qarab standart funksiyalarni yoqish.
// LITE hammaga ochiq; MID/PRO faqat server bergan tier bo'yicha.
// ---------------------------------------------------------------------
static void ApplyTierDefaults()
{
    // --- LITE (hammaga) ---
    CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bEnableVisuals) = true;
    CONFIG_GET_ARRAY(bool, g_Variables.m_PlayerVisuals.m_vecVisualsModifiers,
                     VISUALS_IGNORE_TEAMMATES) = true;
    CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawBox) = true;
    CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawHealthBar) = true;
    CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawWeapon) = true;
    CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawHasC4) = true;
    CONFIG_GET(bool, g_Variables.m_Misc.m_bSniperCrosshair) = true;
    CONFIG_GET(bool, g_Variables.m_SpectatorList.m_bEnableSpectatorList) = true;
    CONFIG_GET(bool, g_Variables.m_Misc.m_bAntiFlash) = true;
    CONFIG_GET(bool, g_Variables.m_Misc.m_bC4Timer) = true;
    CONFIG_GET(bool, g_Variables.m_Misc.m_bGrenadeWarning) = true;
    CONFIG_GET(bool, g_Variables.m_Misc.m_bWatermark) = true;
    CONFIG_GET(bool, g_Variables.m_PlayerGlow.m_bEnableGlow) = true;   // FREE ga kiradi

    // --- MID ---
    if (g_License.HasFeature(ETier::MID))
    {
        CONFIG_GET(bool, g_Variables.m_Bhop.m_bEnableBhop) = true;
        CONFIG_GET(bool, g_Variables.m_TriggerBot.m_bEnableTriggerbot) = true;
    }

    // --- PRO ---
    if (g_License.HasFeature(ETier::PRO))
    {
        CONFIG_GET(bool, g_Variables.m_AimBot.m_bEnableAimbot) = true;
    }
}

enum class EPhase { LOGIN, CONNECTING, LOADING, READY, DONE };

bool LoginWindow::Run()
{
    if (!m_bInitialized) return false;
    UI::ApplyTheme();

    EPhase ePhase = EPhase::LOGIN;
    float flPulse = 0.f;

    // Login form state
    char        szUser[64] = "", szPass[64] = "";
    std::string strError;
    float       flTimer = 0.f;

    // Loading steps
    struct Step {
        const char* name;
        const char* icon;
        float prog; bool done;
    };
    Step steps[] = {
        { "Tizimni tekshirish",       "01", 0, false },
        { "Counter-Strike 2",         "02", 0, false },
        { "Dasturni sozlash",         "03", 0, false },
    };
    int nSteps = 3, nCur = 0;
    bool bCS2Found = false;
    float flCS2CheckTimer = 0.f;

    bool bRunning = true;
    while (bRunning)
    {
        MSG msg;
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg); DispatchMessageW(&msg);
            if (msg.message == WM_QUIT) { bRunning = false; exit(0); }
        }
        if (!bRunning) break;

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGuiIO& io = ImGui::GetIO();
        float dt = io.DeltaTime;
        float W = io.DisplaySize.x, H = io.DisplaySize.y;
        flPulse += dt;

        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
        ImGui::Begin("##Main", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        DrawBackdrop(dl, W, H, flPulse);

        // === CLOSE BUTTON ===
        {
            ImGui::SetCursorPos({ W - 38.f, 12.f });
            ImGui::InvisibleButton("##close", { 26.f, 26.f });
            bool hov = ImGui::IsItemHovered();
            if (ImGui::IsItemClicked()) exit(0);
            ImVec2 mn = ImGui::GetItemRectMin(), mx = ImGui::GetItemRectMax();
            if (hov)
            {
                dl->AddRectFilled(mn, mx, UI::Fade(UI::COL_RED, 0.18f), 3.f);
                dl->AddRect(mn, mx, UI::Fade(UI::COL_RED, 0.7f), 3.f, 0, 1.f);
            }
            ImU32 colX = hov ? UI::COL_RED : UI::COL_TEXT_FAINT;
            dl->AddLine({ mn.x + 8.f, mn.y + 8.f }, { mx.x - 8.f, mx.y - 8.f }, colX, 1.6f);
            dl->AddLine({ mx.x - 8.f, mn.y + 8.f }, { mn.x + 8.f, mx.y - 8.f }, colX, 1.6f);
        }

        // ===============================================================
        //  LOGIN  —  POST {m_strApiUrl}/api/auth/login
        //  Server javobidagi tier (free/mid/pro) huquqlarni belgilaydi.
        // ===============================================================
        if (ePhase == EPhase::LOGIN || ePhase == EPhase::CONNECTING)
        {
            const float fW = 360.f, fX = (W - fW) * 0.5f;
            const bool  bBusy = (ePhase == EPhase::CONNECTING);

            // --- logo mark ---
            {
                ImVec2 c(W * 0.5f, 92.f);
                float p = 0.6f + 0.4f * UI::Pulse(2.f);
                dl->AddNgon(c, 38.f, UI::Fade(UI::COL_CYAN, p), 6, 2.2f);
                dl->AddNgon(c, 28.f, UI::Fade(UI::COL_MAGENTA, 0.45f), 6, 1.3f);
                dl->AddNgon(c, 48.f + 4.f * UI::Pulse(1.4f), UI::Fade(UI::COL_CYAN, 0.13f), 6, 1.f);
                UI::Icon(dl, c, 32.f, UI::ICON_BOLT, UI::Fade(UI::COL_CYAN, p));
            }

            // --- wordmark ---
            {
                if (Fonts::Title) ImGui::PushFont(Fonts::Title);
                ImVec2 s1 = ImGui::CalcTextSize("SHIFTHUB");
                dl->AddText(ImVec2((W - s1.x) * 0.5f, 142.f), UI::COL_TEXT, "SHIFTHUB");
                if (Fonts::Title) ImGui::PopFont();
            }
            {
                char szSub[96];
                snprintf(szSub, sizeof(szSub), "CS2  EXTERNAL   //   v%s", SHIFTHUB_VERSION);
                CenteredText(dl, W, 186.f, szSub, UI::COL_TEXT_FAINT, Fonts::Mono);
            }

            UI::NeonLine(dl, ImVec2(40.f, 214.f), W - 80.f, UI::Fade(UI::COL_CYAN, 0.5f), 1.f);
            CenteredText(dl, W, 224.f, "VIP AKKAUNT", UI::Fade(UI::COL_CYAN, 0.8f), Fonts::Mono);

            if (bBusy) ImGui::BeginDisabled();

            // --- USERNAME ---
            {
                if (Fonts::Small) ImGui::PushFont(Fonts::Small);
                dl->AddText(ImVec2(fX, 248.f), UI::COL_TEXT_MUTE, "USERNAME");
                if (Fonts::Small) ImGui::PopFont();
            }
            ImGui::SetCursorPos({ fX, 266.f });
            ImGui::PushItemWidth(fW);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.f, 9.f));
            bool e1 = ImGui::InputText("##user", szUser, sizeof(szUser),
                ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopStyleVar(); ImGui::PopItemWidth();

            // --- PASSWORD ---
            {
                if (Fonts::Small) ImGui::PushFont(Fonts::Small);
                dl->AddText(ImVec2(fX, 312.f), UI::COL_TEXT_MUTE, "PASSWORD");
                if (Fonts::Small) ImGui::PopFont();
            }
            ImGui::SetCursorPos({ fX, 330.f });
            ImGui::PushItemWidth(fW);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.f, 9.f));
            bool e2 = ImGui::InputText("##pass", szPass, sizeof(szPass),
                ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopStyleVar(); ImGui::PopItemWidth();

            if (bBusy) ImGui::EndDisabled();

            // --- xato xabari ---
            if (!strError.empty())
            {
                if (Fonts::Small) ImGui::PushFont(Fonts::Small);
                ImVec2 ts = ImGui::CalcTextSize(strError.c_str());
                dl->AddText(ImVec2((W - ts.x) * 0.5f, 376.f), UI::COL_RED, strError.c_str());
                if (Fonts::Small) ImGui::PopFont();
            }

            if (ePhase == EPhase::LOGIN)
            {
                // --- KIRISH ---
                {
                    float glow = UI::Pulse(3.f);
                    dl->AddRectFilledMultiColor(ImVec2(fX, 396.f), ImVec2(fX + fW, 398.f),
                        UI::Fade(UI::COL_CYAN, 0.15f + 0.6f * glow),
                        UI::Fade(UI::COL_MAGENTA, 0.15f + 0.6f * glow),
                        UI::Fade(UI::COL_MAGENTA, 0.15f + 0.6f * glow),
                        UI::Fade(UI::COL_CYAN, 0.15f + 0.6f * glow));
                }
                ImGui::SetCursorPos({ fX, 400.f });
                bool bClick = UI::Button("K I R I S H", { fW, 46.f }, UI::BTN_PRIMARY);

                if (bClick || e1 || e2)
                {
                    if (strlen(szUser) > 0 && strlen(szPass) > 0)
                    {
                        strError.clear();
                        ePhase  = EPhase::CONNECTING;
                        flTimer = 0.f;
                    }
                    else
                        strError = "Username va password kiriting!";
                }

                // --- BEPUL REJIM (akkauntsiz) ---
                CenteredText(dl, W, 452.f, "yoki akkauntsiz davom eting",
                    UI::COL_TEXT_FAINT, Fonts::Small);

                ImGui::SetCursorPos({ fX, 470.f });
                if (UI::Button("BEPUL FOYDALANISH", { fW, 40.f }, UI::BTN_GHOST))
                {
                    g_License.m_strUser   = "FreeUser";
                    g_License.m_eTier     = ETier::LITE;
                    g_License.m_strExpiry = "Cheksiz (FREE)";
                    g_License.m_strToken  = "FREE_MODE";

                    ApplyTierDefaults();
                    ePhase = EPhase::LOADING; nCur = 0;
                    for (int i = 0; i < nSteps; i++) { steps[i].prog = 0; steps[i].done = false; }
                    bCS2Found = false;
                }

                CenteredText(dl, W, 522.f, "WH · Glow · AntiFlash · C4 Timer  —  bepul",
                    UI::Fade(UI::COL_GREEN, 0.75f), Fonts::Small);
                CenteredText(dl, W, 542.f, "VIP uchun:  @Shifthubuzbot",
                    UI::COL_TEXT_FAINT, Fonts::Small);
            }
            else // CONNECTING — serverga so'rov
            {
                CenteredText(dl, W, 400.f, "Serverga ulanmoqda...", UI::COL_GREEN, Fonts::Mono);

                flTimer += dt;
                float prog = fminf(flTimer / 1.2f, 1.f);
                dl->AddRectFilled(ImVec2(fX, 428.f), ImVec2(fX + fW, 433.f),
                    UI::Fade(UI::COL_TEXT_FAINT, 0.35f), 2.f);
                dl->AddRectFilled(ImVec2(fX, 428.f), ImVec2(fX + fW * prog, 433.f),
                    UI::COL_CYAN, 2.f);

                if (flTimer >= 1.2f)
                {
                    json jBody;
                    jBody["username"] = std::string(szUser);
                    jBody["password"] = std::string(szPass);

                    Http::Response resp = Http::Post(
                        g_License.m_strApiUrl + X("/api/auth/login"), jBody.dump());

                    if (!resp.success || resp.body.empty())
                    {
                        strError = (resp.statusCode == 0)
                            ? "Server bilan bog'lanib bo'lmadi!"
                            : "Login xato (kod: " + std::to_string(resp.statusCode) + ")";
                        try { json j = json::parse(resp.body); strError = j.value("error", strError); }
                        catch (...) {}
                        ePhase = EPhase::LOGIN;
                    }
                    else
                    {
                        try
                        {
                            json jr = json::parse(resp.body);
                            g_License.m_strToken  = jr.value("token", "");
                            g_License.m_strUser   = jr["user"].value("username", std::string(szUser));
                            std::string t         = jr["user"].value("tier", "free");
                            g_License.m_eTier     = (t == "pro" || t == "vip") ? ETier::PRO
                                                  : (t == "mid") ? ETier::MID : ETier::LITE;
                            g_License.m_strExpiry = jr["user"].value("expires_at", "N/A");

                            // parol xotirada qolmasin
                            SecureZeroMemory(szPass, sizeof(szPass));

                            ApplyTierDefaults();
                            ePhase = EPhase::LOADING; nCur = 0;
                            for (int i = 0; i < nSteps; i++) { steps[i].prog = 0; steps[i].done = false; }
                            bCS2Found = false;
                        }
                        catch (...) { strError = "Server javobi xato!"; ePhase = EPhase::LOGIN; }
                    }
                }
            }

            // --- footer ---
            CenteredText(dl, W, H - 52.f, "shifthub.uz", UI::Fade(UI::COL_CYAN, 0.6f), Fonts::Mono);
            CenteredText(dl, W, H - 32.f, "SHIFTHUB", UI::COL_TEXT_FAINT, Fonts::Small);
        }

        // ===============================================================
        //  LOADING / READY
        // ===============================================================
        else if (ePhase == EPhase::LOADING || ePhase == EPhase::READY)
        {
            // --- HEADER ---
            {
                ImVec2 c(W * 0.5f, 46.f);
                float p = 0.6f + 0.4f * UI::Pulse(2.f);
                dl->AddNgon(c, 20.f, UI::Fade(UI::COL_CYAN, p), 6, 1.6f);
                UI::Icon(dl, c, 17.f, UI::ICON_BOLT, UI::Fade(UI::COL_CYAN, p));
            }
            {
                char szHdr[64];
                snprintf(szHdr, sizeof(szHdr), "SHIFTHUB  v%s", SHIFTHUB_VERSION);
                CenteredText(dl, W, 74.f, szHdr, UI::COL_TEXT, Fonts::Mono);
            }
            UI::NeonLine(dl, ImVec2(40.f, 96.f), W - 80.f, UI::Fade(UI::COL_CYAN, 0.45f), 1.f);

            // --- CROSSHAIR + GAME ---
            {
                float cx = W * 0.5f, cy = 122.f;
                UI::Icon(dl, ImVec2(cx, cy), 26.f, UI::ICON_CROSSHAIR, UI::Fade(UI::COL_CYAN, 0.8f));
                CenteredText(dl, W, 142.f, "COUNTER-STRIKE 2", UI::COL_TEXT_FAINT, Fonts::Small);
            }

            // --- USER STRIP ---
            {
                std::string nameUp = g_License.m_strUser;
                for (auto& ch : nameUp) ch = (char)toupper((unsigned char)ch);

                ImU32 colTier = ImGui::ColorConvertFloat4ToU32(g_License.GetTierColor());
                float wTier = UI::ChipWidth(g_License.GetTierName());
                float wName = UI::ChipWidth(nameUp.c_str());
                float wExp  = UI::ChipWidth(g_License.m_strExpiry.c_str());
                float total = wTier + wName + wExp + 20.f;
                float x = (W - total) * 0.5f;

                UI::Chip(g_License.GetTierName(), colTier, ImVec2(x, 168.f));
                UI::Chip(nameUp.c_str(), UI::COL_TEXT, ImVec2(x + wTier + 10.f, 168.f));
                UI::Chip(g_License.m_strExpiry.c_str(), UI::COL_TEXT_MUTE, ImVec2(x + wTier + wName + 20.f, 168.f));
            }

            // ===== LOADING STEPS =====
            float sX = 44.f, sW = W - 88.f;
            float sY = 214.f;

            for (int i = 0; i < nSteps; i++)
            {
                float rowY = sY + i * 74.f;
                ImVec2 mn(sX, rowY), mx(sX + sW, rowY + 60.f);

                bool bDone = steps[i].done;
                bool bCurr = (i == nCur) && !bDone;
                ImU32 accent = bDone ? UI::COL_GREEN : (bCurr ? UI::COL_CYAN : IM_COL32(40, 52, 70, 255));

                dl->AddRectFilled(mn, mx, bCurr ? IM_COL32(12, 20, 30, 235) : IM_COL32(10, 14, 21, 220), 4.f);
                dl->AddRect(mn, mx, UI::Fade(accent, bDone || bCurr ? 0.55f : 0.35f), 4.f, 0, 1.f);
                dl->AddRectFilled(mn, ImVec2(mn.x + 2.5f, mx.y), accent, 1.f);
                if (bCurr) UI::GlowRect(dl, mn, mx, UI::COL_CYAN, 4.f, 4, 0.8f);

                // index
                if (Fonts::Mono) ImGui::PushFont(Fonts::Mono);
                dl->AddText(ImVec2(mn.x + 14.f, rowY + 10.f), UI::Fade(accent, 0.9f), steps[i].icon);
                if (Fonts::Mono) ImGui::PopFont();

                // name
                dl->AddText(ImVec2(mn.x + 48.f, rowY + 9.f),
                    bDone ? UI::COL_GREEN : (bCurr ? UI::COL_TEXT : UI::COL_TEXT_FAINT), steps[i].name);

                // status
                char szStat[16];
                if (bDone)      snprintf(szStat, sizeof(szStat), "OK");
                else if (bCurr) snprintf(szStat, sizeof(szStat), "%d%%", (int)(steps[i].prog * 100));
                else            snprintf(szStat, sizeof(szStat), "--");
                ImVec2 ss = ImGui::CalcTextSize(szStat);
                dl->AddText(ImVec2(mx.x - ss.x - 14.f, rowY + 9.f),
                    bDone ? UI::COL_GREEN : (bCurr ? UI::COL_CYAN : UI::COL_TEXT_FAINT), szStat);

                // spinner while waiting
                if (bCurr && i == 1 && !bCS2Found)
                {
                    float a = flPulse * 4.f;
                    ImVec2 sc(mx.x - 34.f, rowY + 40.f);
                    dl->PathArcTo(sc, 6.f, a, a + 4.2f, 16);
                    dl->PathStroke(UI::COL_AMBER, 0, 1.6f);
                }

                // progress rail
                float barY = rowY + 46.f;
                dl->AddRectFilled(ImVec2(mn.x + 14.f, barY), ImVec2(mx.x - 14.f, barY + 4.f), IM_COL32(16, 22, 32, 255), 2.f);
                float fillW = (sW - 28.f) * ImClamp(steps[i].prog, 0.f, 1.f);
                if (fillW > 0.f)
                {
                    dl->AddRectFilledMultiColor(ImVec2(mn.x + 14.f, barY), ImVec2(mn.x + 14.f + fillW, barY + 4.f),
                        UI::Fade(accent, 0.5f), accent, accent, UI::Fade(accent, 0.5f));
                }

                if (bCurr && i == 1 && !bCS2Found)
                    dl->AddText(ImVec2(mn.x + 48.f, rowY + 28.f), UI::COL_AMBER, "CS2 ni oching — kutilmoqda...");
            }

            // ===== ANIMATE STEPS =====
            if (nCur < nSteps && !steps[nCur].done)
            {
                if (nCur == 1) // Counter-Strike 2 — wait for CS2
                {
                    flCS2CheckTimer += dt;
                    if (flCS2CheckTimer >= 0.5f) // check every 500ms
                    {
                        flCS2CheckTimer = 0;
                        bCS2Found = IsCS2Running();
                    }

                    if (bCS2Found)
                    {
                        steps[nCur].prog += dt / 0.5f; // fast fill when found
                        if (steps[nCur].prog >= 1.f)
                        { steps[nCur].prog = 1; steps[nCur].done = true; nCur++; }
                    }
                    else
                    {
                        // Pulsing bar to show waiting
                        steps[nCur].prog = (sinf(flPulse * 3.f) + 1.f) * 0.15f + 0.05f;
                    }
                }
                else // Other steps — auto progress
                {
                    float speed = (nCur == 0) ? 1.0f : 0.6f;
                    steps[nCur].prog += dt / speed;
                    if (steps[nCur].prog >= 1.f)
                    {
                        steps[nCur].prog = 1; steps[nCur].done = true;

                        // Real actions
                        if (nCur == 0 && !g_License.m_strToken.empty()
                            && g_License.m_strToken != "LOCAL"
                            && g_License.m_strToken != "FREE_MODE")
                        {
                            g_License.CheckLicense();
                            g_License.DownloadDependencies();
                        }

                        nCur++;
                    }
                }
            }

            // After last step done → READY
            if (nCur >= nSteps && ePhase == EPhase::LOADING)
                ePhase = EPhase::READY;

            // === BOSHLASH (when READY) ===
            if (ePhase == EPhase::READY)
            {
                float btnW = W - 88.f, btnX = 44.f, btnY = H - 134.f;

                float glow = UI::Pulse(3.f);
                dl->AddRectFilledMultiColor(ImVec2(btnX, btnY - 4.f), ImVec2(btnX + btnW, btnY - 2.f),
                    UI::Fade(UI::COL_CYAN, 0.15f + 0.6f * glow), UI::Fade(UI::COL_MAGENTA, 0.15f + 0.6f * glow),
                    UI::Fade(UI::COL_MAGENTA, 0.15f + 0.6f * glow), UI::Fade(UI::COL_CYAN, 0.15f + 0.6f * glow));

                ImGui::SetCursorPos({ btnX, btnY });
                if (UI::Button("B O S H L A S H", { btnW, 46.f }, UI::BTN_PRIMARY))
                    ePhase = EPhase::DONE;

                // === MENU KEYBIND ===
                int& menuKey = CONFIG_GET(int, g_Variables.m_Gui.m_iMenuKey);
                static bool bListeningMenuKey = false;
                static float fWaitTimer = 0.f;

                if (bListeningMenuKey)
                {
                    CenteredText(dl, W, btnY + 58.f, "[ Istalgan tugmani bosing...  ESC = bekor ]", UI::COL_AMBER, Fonts::Mono);

                    fWaitTimer -= dt;
                    if (fWaitTimer < 4.8f) // delay 0.2s to ignore mouse click
                    {
                        for (int i = 1; i < 256; i++) {
                            if (GetAsyncKeyState(i) & 0x8000) {
                                if (i != VK_ESCAPE) menuKey = i;
                                bListeningMenuKey = false;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    char buf[96];
                    snprintf(buf, sizeof(buf), "MENYU TUGMASI:   [ %s ]", UI::KeyName(menuKey));

                    if (Fonts::Mono) ImGui::PushFont(Fonts::Mono);
                    ImVec2 ts = ImGui::CalcTextSize(buf);
                    if (Fonts::Mono) ImGui::PopFont();

                    ImGui::SetCursorPos({ (W - ts.x) * 0.5f - 10.f, btnY + 54.f });
                    ImGui::InvisibleButton("##menukeybtn", { ts.x + 20.f, ts.y + 8.f });
                    bool hov = ImGui::IsItemHovered();
                    if (hov) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                    if (ImGui::IsItemClicked()) { bListeningMenuKey = true; fWaitTimer = 5.0f; }

                    ImVec2 mn = ImGui::GetItemRectMin(), mx = ImGui::GetItemRectMax();
                    if (hov) dl->AddRect(mn, mx, UI::Fade(UI::COL_CYAN, 0.45f), 3.f, 0, 1.f);

                    if (Fonts::Mono) ImGui::PushFont(Fonts::Mono);
                    dl->AddText(ImVec2(mn.x + 10.f, mn.y + 4.f), hov ? UI::COL_CYAN : UI::COL_TEXT_MUTE, buf);
                    if (Fonts::Mono) ImGui::PopFont();
                }
            }

            // --- footer ---
            CenteredText(dl, W, H - 46.f, "shifthub.uz", UI::Fade(UI::COL_CYAN, 0.6f), Fonts::Mono);
            CenteredText(dl, W, H - 26.f, "SHIFTHUB  ·  @Bakoev_71", UI::COL_TEXT_FAINT, Fonts::Small);
        }

        // ===============================================================
        //  DONE
        // ===============================================================
        else if (ePhase == EPhase::DONE)
        {
            ImGui::End();
            ImGui::PopStyleVar();
            ImGui::Render();
            float cc[4] = { 0.03f, 0.04f, 0.06f, 1 };
            m_pContext->OMSetRenderTargets(1, &m_pRTV, NULL);
            m_pContext->ClearRenderTargetView(m_pRTV, cc);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            m_pSwapChain->Present(1, 0);
            break;
        }

        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::Render();
        float cc[4] = { 0.03f, 0.04f, 0.06f, 1 };
        m_pContext->OMSetRenderTargets(1, &m_pRTV, NULL);
        m_pContext->ClearRenderTargetView(m_pRTV, cc);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        m_pSwapChain->Present(1, 0);
    }

    return true;
}

// ===================================================================
void LoginWindow::Destroy()
{
    if (!m_bInitialized) return;
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    if (m_pRTV) { m_pRTV->Release(); m_pRTV = nullptr; }
    if (m_pSwapChain) { m_pSwapChain->Release(); m_pSwapChain = nullptr; }
    if (m_pContext) { m_pContext->Release(); m_pContext = nullptr; }
    if (m_pDevice) { m_pDevice->Release(); m_pDevice = nullptr; }
    if (m_hWnd) { DestroyWindow(m_hWnd); m_hWnd = nullptr; }
    UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);
    m_bInitialized = false;
}
