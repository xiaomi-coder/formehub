#include "../Includes.h"
#include <json.hpp>
using json = nlohmann::json;

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
bool LoginWindow::Create()
{
    if (m_bInitialized) return true;
    int wndW = 480, wndH = 580;
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
    ImFontConfig cfg = {};
    cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags_Bold;
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 16, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    ImGuiFreeType::BuildFontAtlas(io.Fonts, 0);

    m_bInitialized = true;
    return true;
}

// ===================================================================
static void ApplyTheme()
{
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 0; s.FrameRounding = 4; s.GrabRounding = 3;
    s.WindowBorderSize = 0; s.FrameBorderSize = 1;
    s.ItemSpacing = { 8, 8 }; s.FramePadding = { 10, 8 };
    auto* c = s.Colors;
    c[ImGuiCol_WindowBg]       = { 0.04f, 0.05f, 0.08f, 1.0f };
    c[ImGuiCol_FrameBg]        = { 0.07f, 0.09f, 0.13f, 1.0f };
    c[ImGuiCol_FrameBgHovered] = { 0.09f, 0.12f, 0.17f, 1.0f };
    c[ImGuiCol_FrameBgActive]  = { 0.0f,  0.5f,  0.2f,  0.3f };
    c[ImGuiCol_Border]         = { 0.0f,  0.45f, 0.18f, 0.5f };
    c[ImGuiCol_Text]           = { 0.85f, 0.95f, 0.88f, 1.0f };
    c[ImGuiCol_TextDisabled]   = { 0.30f, 0.42f, 0.35f, 1.0f };
    c[ImGuiCol_Button]         = { 0.0f,  0.42f, 0.16f, 1.0f };
    c[ImGuiCol_ButtonHovered]  = { 0.0f,  0.58f, 0.22f, 1.0f };
    c[ImGuiCol_ButtonActive]   = { 0.0f,  0.72f, 0.28f, 1.0f };
    c[ImGuiCol_CheckMark]      = { 0.0f,  1.0f,  0.35f, 1.0f };
}

// ===================================================================
enum class EPhase { LOGIN, CONNECTING, LOADING, READY, DONE };

bool LoginWindow::Run()
{
    if (!m_bInitialized) return false;
    ApplyTheme();

    EPhase ePhase = EPhase::LOGIN;
    char szUser[64] = "", szPass[64] = "";
    std::string strError;
    float flTimer = 0.f, flPulse = 0.f;

    // Loading steps
    struct Step {
        const char* name;
        const char* icon;
        float prog; bool done;
    };
    Step steps[] = {
        { "Litsenziyani tekshirish",  "[1/3]", 0, false },
        { "Counter-Strike 2",         "[2/3]", 0, false },
        { "Dasturni sozlash",         "[3/3]", 0, false },
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
        ImGui::Begin("##Main", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoCollapse);

        ImDrawList* dl = ImGui::GetWindowDrawList();

        // === GREEN TOP ACCENT ===
        dl->AddRectFilled({ 0, 0 }, { W, 3 }, IM_COL32(0, 200, 65, 255));

        // === X BUTTON (top-right) ===
        {
            ImGui::SetCursorPos({ W - 35, 8 });
            ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.1f, 0.1f, 0.5f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.5f, 0.1f, 0.1f, 0.8f });
            ImGui::PushStyleColor(ImGuiCol_Text, { 0.5f, 0.5f, 0.5f, 1.0f });
            if (ImGui::Button("X", { 25, 25 })) exit(0);
            ImGui::PopStyleColor(4);
        }

        // ===============================================================
        //  LOGIN SCREEN
        // ===============================================================
        if (ePhase == EPhase::LOGIN || ePhase == EPhase::CONNECTING)
        {
            float fW = 340.f, fX = (W - fW) * 0.5f;

            // Header
            ImGui::SetCursorPosY(30);
            {
                const char* t = "S H I F T H U B";
                ImGui::SetCursorPosX((W - ImGui::CalcTextSize(t).x) * 0.5f);
                ImGui::PushStyleColor(ImGuiCol_Text, { 0.0f, 0.85f, 0.30f, 1.0f });
                ImGui::Text("%s", t);
                ImGui::PopStyleColor();
            }
            {
                char szSub[64];
                snprintf(szSub, sizeof(szSub), "Professional CS2 Software  |  v%s", SHIFTHUB_VERSION);
                const char* t = szSub;
                ImGui::SetCursorPosX((W - ImGui::CalcTextSize(t).x) * 0.5f);
                ImGui::PushStyleColor(ImGuiCol_Text, { 0.28f, 0.42f, 0.35f, 1.0f });
                ImGui::Text("%s", t);
                ImGui::PopStyleColor();
            }
            dl->AddLine({ 30, 82 }, { W - 30, 82 }, IM_COL32(0, 110, 40, 100));

            // Login title
            ImGui::SetCursorPos({ 0, 100 });
            {
                const char* t = "// LOGIN //";
                ImGui::SetCursorPosX((W - ImGui::CalcTextSize(t).x) * 0.5f);
                ImGui::PushStyleColor(ImGuiCol_Text, { 0.0f, 0.78f, 0.28f, 1.0f });
                ImGui::Text("%s", t); ImGui::PopStyleColor();
            }

            ImGui::SetCursorPos({ fX, 140 });

            // USERNAME
            ImGui::PushStyleColor(ImGuiCol_Text, { 0.45f, 0.65f, 0.55f, 1 }); ImGui::Text("USERNAME"); ImGui::PopStyleColor();
            ImGui::SetCursorPosX(fX); ImGui::PushItemWidth(fW);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 12, 10 });
            bool e1 = ImGui::InputText("##user", szUser, sizeof(szUser), ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopStyleVar(); ImGui::PopItemWidth();
            ImGui::Spacing(); ImGui::Spacing();

            // PASSWORD
            ImGui::SetCursorPosX(fX);
            ImGui::PushStyleColor(ImGuiCol_Text, { 0.45f, 0.65f, 0.55f, 1 }); ImGui::Text("PASSWORD"); ImGui::PopStyleColor();
            ImGui::SetCursorPosX(fX); ImGui::PushItemWidth(fW);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 12, 10 });
            bool e2 = ImGui::InputText("##pass", szPass, sizeof(szPass),
                ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopStyleVar(); ImGui::PopItemWidth();
            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

            if (ePhase == EPhase::LOGIN)
            {
                // KIRISH BUTTON
                ImGui::SetCursorPosX(fX);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 14 });
                bool bClick = ImGui::Button("K I R I S H", { fW, 0 });
                ImGui::PopStyleVar();

                if ((bClick || e1 || e2) && strlen(szUser) > 0 && strlen(szPass) > 0)
                { strError.clear(); ePhase = EPhase::CONNECTING; flTimer = 0; }
                else if (bClick || e1 || e2)
                    strError = "Username va password kiriting!";

                // Telegram
                ImGui::Spacing();
                {
                    const char* tg = "Adminga bog'lanish: @bakoev_71";
                    ImGui::SetCursorPosX((W - ImGui::CalcTextSize(tg).x) * 0.5f);
                    ImGui::PushStyleColor(ImGuiCol_Text, { 0.25f, 0.40f, 0.32f, 1 });
                    ImGui::Text("%s", tg); ImGui::PopStyleColor();
                }

                // FREE VERSION BUTTON
                ImGui::Spacing(); ImGui::Spacing();
                ImGui::SetCursorPosX(fX);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 10 });
                ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.2f, 0.15f, 1 });
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.15f, 0.3f, 0.2f, 1 });
                ImGui::PushStyleColor(ImGuiCol_Text, { 0.6f, 0.8f, 0.7f, 1 });
                if (ImGui::Button("FREE o'ynamoq!", { fW, 0 }))
                {
                    g_License.m_strUser = "FreeUser";
                    g_License.m_eTier = ETier::LITE; // Free tier
                    g_License.m_strExpiry = "Cheksiz (FREE)";
                    g_License.m_strToken = "FREE_MODE";
                    ePhase = EPhase::LOADING; nCur = 0;
                    for (int i = 0; i < nSteps; i++) { steps[i].prog = 0; steps[i].done = false; }
                    bCS2Found = false;

                    // Apply FREE mode restrictions automatically
                    CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bEnableVisuals) = true; // MUST ENABLE THIS FOR WH
                    CONFIG_GET_ARRAY(bool, g_Variables.m_PlayerVisuals.m_vecVisualsModifiers, VISUALS_IGNORE_TEAMMATES) = true; // Jamoani e'tiborsiz qoldirish
                    CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawBox) = true;
                    CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawHealthBar) = true;
                    CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawWeapon) = true;
                    CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bDrawHasC4) = true;
                    CONFIG_GET(bool, g_Variables.m_Misc.m_bSniperCrosshair) = true;
                    CONFIG_GET(bool, g_Variables.m_SpectatorList.m_bEnableSpectatorList) = true; // Automatically show spectator list
                    CONFIG_GET(bool, g_Variables.m_Misc.m_bAntiFlash) = true;   // Flash himoya
                    CONFIG_GET(bool, g_Variables.m_Misc.m_bC4Timer) = true;     // C4 ogohlantiruvchi
                    CONFIG_GET(bool, g_Variables.m_Misc.m_bGrenadeWarning) = true; // Grenade warning
                }
                ImGui::PopStyleColor(3);
                ImGui::PopStyleVar();
            }
            else // CONNECTING
            {
                ImGui::SetCursorPosX(fX);
                ImGui::PushStyleColor(ImGuiCol_Text, { 0, 0.78f, 0.28f, 1 });
                ImGui::Text("Serverga ulanmoqda..."); ImGui::PopStyleColor();

                flTimer += dt; float prog = fminf(flTimer / 1.5f, 1.f);
                ImGui::SetCursorPosX(fX); ImVec2 bp = ImGui::GetCursorScreenPos();
                dl->AddRectFilled(bp, { bp.x + fW, bp.y + 5 }, IM_COL32(25, 32, 42, 255), 2);
                dl->AddRectFilled(bp, { bp.x + fW * prog, bp.y + 5 }, IM_COL32(0, 200, 65, 255), 2);

                if (flTimer >= 1.5f)
                {
                    json jBody; jBody["username"] = std::string(szUser); jBody["password"] = std::string(szPass);
                    Http::Response resp = Http::Post(g_License.m_strApiUrl + "/api/auth/login", jBody.dump());

                    if (!resp.success || resp.body.empty())
                    {
                        strError = (resp.statusCode == 0) ? "Server bilan bog'lanib bo'lmadi!"
                            : "Login xato (kod: " + std::to_string(resp.statusCode) + ")";
                        try { json j = json::parse(resp.body); strError = j.value("error", strError); } catch (...) {}
                        ePhase = EPhase::LOGIN;
                    }
                    else
                    {
                        try {
                            json jr = json::parse(resp.body);
                            g_License.m_strToken = jr.value("token", "");
                            g_License.m_strUser = jr["user"].value("username", std::string(szUser));
                            std::string t = jr["user"].value("tier", "free");
                            g_License.m_eTier = (t == "pro") ? ETier::PRO : (t == "mid") ? ETier::MID : ETier::LITE;
                            g_License.m_strExpiry = jr["user"].value("expires_at", "N/A");
                            ePhase = EPhase::LOADING; nCur = 0;
                            for (int i = 0; i < nSteps; i++) { steps[i].prog = 0; steps[i].done = false; }
                            bCS2Found = false;
                        } catch (...) { strError = "Server javobi xato!"; ePhase = EPhase::LOGIN; }
                    }
                }
            }

            // Error
            if (!strError.empty())
            {
                ImGui::Spacing();
                ImGui::SetCursorPosX((W - ImGui::CalcTextSize(strError.c_str()).x) * 0.5f);
                ImGui::PushStyleColor(ImGuiCol_Text, { 1, 0.22f, 0.18f, 1 });
                ImGui::TextWrapped("%s", strError.c_str()); ImGui::PopStyleColor();
            }

            // Bottom
            {
                const char* by = "BY: Bissikoo";
                ImGui::SetCursorPos({ (W - ImGui::CalcTextSize(by).x) * 0.5f, H - 52 });
                ImGui::PushStyleColor(ImGuiCol_Text, { 0, 0.60f, 0.22f, 0.7f });
                ImGui::Text("%s", by); ImGui::PopStyleColor();
                const char* site = "shifthub.uz";
                ImGui::SetCursorPos({ (W - ImGui::CalcTextSize(site).x) * 0.5f, H - 32 });
                ImGui::PushStyleColor(ImGuiCol_Text, { 0.22f, 0.35f, 0.28f, 1 });
                ImGui::Text("%s", site); ImGui::PopStyleColor();
            }
        }

        // ===============================================================
        //  LOADING SCREEN (unique horizontal design)
        // ===============================================================
        else if (ePhase == EPhase::LOADING || ePhase == EPhase::READY)
        {
            // --- HEADER ---
            ImGui::SetCursorPosY(18);
            {
                char szHdr[64];
                snprintf(szHdr, sizeof(szHdr), "S H I F T H U B  v%s", SHIFTHUB_VERSION);
                const char* t = szHdr;
                ImGui::SetCursorPosX((W - ImGui::CalcTextSize(t).x) * 0.5f);
                ImGui::PushStyleColor(ImGuiCol_Text, { 0, 0.82f, 0.30f, 1 });
                ImGui::Text("%s", t); ImGui::PopStyleColor();
            }
            dl->AddLine({ 30, 42 }, { W - 30, 42 }, IM_COL32(0, 100, 40, 80));

            // --- CS2 CROSSHAIR ---
            {
                float cx = W * 0.5f, cy = 65;
                ImU32 colC = IM_COL32(0, 200, 65, 140);
                dl->AddLine({ cx - 12, cy }, { cx - 4, cy }, colC, 2);
                dl->AddLine({ cx + 4, cy }, { cx + 12, cy }, colC, 2);
                dl->AddLine({ cx, cy - 12 }, { cx, cy - 4 }, colC, 2);
                dl->AddLine({ cx, cy + 4 }, { cx, cy + 12 }, colC, 2);
                dl->AddCircleFilled({ cx, cy }, 2, IM_COL32(0, 255, 80, 200));
            }
            {
                const char* cs2 = "COUNTER-STRIKE 2";
                ImGui::SetCursorPos({ (W - ImGui::CalcTextSize(cs2).x) * 0.5f, 82 });
                ImGui::PushStyleColor(ImGuiCol_Text, { 0.30f, 0.48f, 0.38f, 0.8f });
                ImGui::Text("%s", cs2); ImGui::PopStyleColor();
            }

            // --- USER INFO ---
            dl->AddLine({ 60, 106 }, { W - 60, 106 }, IM_COL32(0, 80, 35, 50));
            ImGui::SetCursorPosY(114);
            {
                std::string nameUp = g_License.m_strUser;
                for (auto& ch : nameUp) ch = (char)toupper((unsigned char)ch);
                char info[128];
                snprintf(info, sizeof(info), "%s  |  %s  |  %s", nameUp.c_str(), g_License.GetTierName(), g_License.m_strExpiry.c_str());
                ImGui::SetCursorPosX((W - ImGui::CalcTextSize(info).x) * 0.5f);
                ImGui::PushStyleColor(ImGuiCol_Text, g_License.GetTierColor());
                ImGui::Text("%s", info); ImGui::PopStyleColor();
            }
            dl->AddLine({ 60, 138 }, { W - 60, 138 }, IM_COL32(0, 80, 35, 50));

            // ===== LOADING STEPS (horizontal bars — unique design) =====
            float sX = 40, sW = W - 80;
            float sY = 160;

            for (int i = 0; i < nSteps; i++)
            {
                float rowY = sY + i * 68;

                // Step card background
                ImU32 bgCol = steps[i].done ? IM_COL32(0, 40, 18, 120) :
                    (i == nCur ? IM_COL32(0, 35, 15, 100) : IM_COL32(12, 15, 22, 80));
                dl->AddRectFilled({ sX, rowY }, { sX + sW, rowY + 56 }, bgCol, 4);

                // Left border accent
                ImU32 accentCol = steps[i].done ? IM_COL32(0, 220, 70, 255) :
                    (i == nCur ? IM_COL32(0, 160, 55, 200) : IM_COL32(40, 55, 45, 100));
                dl->AddRectFilled({ sX, rowY }, { sX + 3, rowY + 56 }, accentCol, 2);

                // Icon/number
                ImGui::SetCursorPos({ sX + 14, rowY + 6 });
                ImGui::PushStyleColor(ImGuiCol_Text, { 0.20f, 0.35f, 0.28f, 1 });
                ImGui::Text("%s", steps[i].icon);
                ImGui::PopStyleColor();

                // Name
                ImGui::SetCursorPos({ sX + 60, rowY + 6 });
                ImVec4 nameCol = steps[i].done ? ImVec4(0, 0.88f, 0.32f, 1) :
                    (i == nCur ? ImVec4(0.85f, 0.95f, 0.88f, 1) : ImVec4(0.30f, 0.42f, 0.36f, 1));
                ImGui::PushStyleColor(ImGuiCol_Text, nameCol);
                ImGui::Text("%s", steps[i].name);
                ImGui::PopStyleColor();

                // Status
                ImGui::SetCursorPos({ sX + sW - 80, rowY + 6 });
                if (steps[i].done)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, { 0, 0.85f, 0.30f, 1 });
                    ImGui::Text("100%%");
                }
                else if (i == nCur)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, { 0.6f, 0.8f, 0.7f, 1 });
                    ImGui::Text("%d%%", (int)(steps[i].prog * 100));
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, { 0.25f, 0.35f, 0.30f, 1 });
                    ImGui::Text("---");
                }
                ImGui::PopStyleColor();

                // Progress bar (full width, thin)
                float barY = rowY + 42;
                dl->AddRectFilled({ sX + 12, barY }, { sX + sW - 12, barY + 4 }, IM_COL32(18, 22, 32, 200), 2);
                float fillW = (sW - 24) * steps[i].prog;
                if (fillW > 0)
                    dl->AddRectFilled({ sX + 12, barY }, { sX + 12 + fillW, barY + 4 }, IM_COL32(0, 200, 65, 255), 2);
            }

            // ===== ANIMATE STEPS =====
            if (nCur < nSteps && !steps[nCur].done)
            {
                if (nCur == 1) // Counter-Strike 2 — wait for CS2 (Now step 2, index 1)
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
                        float pulse = (sinf(flPulse * 3.f) + 1.f) * 0.15f + 0.05f;
                        steps[nCur].prog = pulse;

                        // Show message
                        ImGui::SetCursorPos({ sX + 60, sY + 1 * 68 + 22 }); // Adjusted for step 2
                        ImGui::PushStyleColor(ImGuiCol_Text, { 0.7f, 0.5f, 0.2f, 1 });
                        ImGui::Text("CS2 ni oching! Kutilmoqda...");
                        ImGui::PopStyleColor();
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
                        if (nCur == 0) g_License.CheckLicense();
                        // Removed DownloadDependencies

                        nCur++;
                    }
                }
            }

            // After step 3 (Dastur) done → READY
            if (nCur >= nSteps && ePhase == EPhase::LOADING)
                ePhase = EPhase::READY;

            // === BOSHLASH (when READY) ===
            if (ePhase == EPhase::READY)
            {
                float btnW = W - 100, btnX = 50, btnY = H - 110; // moved up to fit keybind

                // Animated green line
                float lineAlpha = (sinf(flPulse * 4.f) + 1.f) * 0.5f;
                dl->AddRectFilled({ btnX, btnY - 3 }, { btnX + btnW, btnY },
                    IM_COL32(0, 200, 65, (int)(lineAlpha * 200)), 1);

                ImGui::SetCursorPos({ btnX, btnY });
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 14 });
                if (ImGui::Button("B O S H L A S H", { btnW, 0 }))
                    ePhase = EPhase::DONE;
                ImGui::PopStyleVar();

                // === MENU KEYBIND ===
                int& menuKey = CONFIG_GET(int, g_Variables.m_Gui.m_iMenuKey);
                static bool bListeningMenuKey = false;
                static float fWaitTimer = 0.f;

                ImGui::SetCursorPosY(btnY + 45); // below boshlash
                if (bListeningMenuKey)
                {
                    const char* lb = "[ Istalgan tugmani bosing... Esc=Bekor ]";
                    ImGui::SetCursorPosX((W - ImGui::CalcTextSize(lb).x) * 0.5f);
                    ImGui::PushStyleColor(ImGuiCol_Text, { 0.8f, 0.4f, 0.1f, 1 });
                    ImGui::Text("%s", lb);
                    ImGui::PopStyleColor();

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
                    // Basic VK2Str
                    char keyName[32] = "Insert";
                    if (menuKey == VK_DELETE) strcpy(keyName, "Delete");
                    else if (menuKey == VK_HOME) strcpy(keyName, "Home");
                    else if (menuKey == VK_END) strcpy(keyName, "End");
                    else if (menuKey == VK_F1) strcpy(keyName, "F1");
                    else if (menuKey == VK_F2) strcpy(keyName, "F2");
                    else if (menuKey == VK_F3) strcpy(keyName, "F3");
                    else if (menuKey == VK_F4) strcpy(keyName, "F4");
                    else if (menuKey == VK_F5) strcpy(keyName, "F5");
                    else if (menuKey == VK_F6) strcpy(keyName, "F6");
                    else if (menuKey == VK_F7) strcpy(keyName, "F7");
                    else if (menuKey == VK_F8) strcpy(keyName, "F8");
                    else if (menuKey >= 'A' && menuKey <= 'Z') { keyName[0] = (char)menuKey; keyName[1] = 0; }
                    else if (menuKey >= '0' && menuKey <= '9') { keyName[0] = (char)menuKey; keyName[1] = 0; }
                    else if (menuKey == VK_LMENU || menuKey == VK_RMENU || menuKey == VK_MENU) strcpy(keyName, "Alt");
                    else if (menuKey == VK_LSHIFT || menuKey == VK_RSHIFT || menuKey == VK_SHIFT) strcpy(keyName, "Shift");
                    else if (menuKey == VK_LCONTROL || menuKey == VK_RCONTROL || menuKey == VK_CONTROL) strcpy(keyName, "Ctrl");
                    else if (menuKey == VK_MBUTTON) strcpy(keyName, "M3");
                    else if (menuKey == VK_XBUTTON1) strcpy(keyName, "M4");
                    else if (menuKey == VK_XBUTTON2) strcpy(keyName, "M5");
                    else if (menuKey != VK_INSERT) snprintf(keyName, sizeof(keyName), "Key: %d", menuKey);

                    char buf[64];
                    snprintf(buf, sizeof(buf), "Menyuni ekranga chiqarish tugmasi:  [ %s ]", keyName);
                    
                    ImGui::SetCursorPosX((W - ImGui::CalcTextSize(buf).x) * 0.5f);
                    ImGui::PushStyleColor(ImGuiCol_Text, { 0.4f, 0.6f, 0.5f, 1 });
                    ImGui::Text("%s", buf);
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                        ImGui::PopStyleColor();
                        ImGui::PushStyleColor(ImGuiCol_Text, { 0.6f, 0.8f, 0.7f, 1 });
                        ImGui::SetCursorPos({(W - ImGui::CalcTextSize(buf).x) * 0.5f, btnY + 45});
                        ImGui::Text("%s", buf);
                    }
                    ImGui::PopStyleColor();

                    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                    {
                        bListeningMenuKey = true;
                        fWaitTimer = 5.0f;
                    }
                }
            }

            // Bottom
            {
                const char* by = "BY: Bissikoo";
                ImGui::SetCursorPos({ (W - ImGui::CalcTextSize(by).x) * 0.5f, H - 45 });
                ImGui::PushStyleColor(ImGuiCol_Text, { 0, 0.60f, 0.22f, 0.7f });
                ImGui::Text("%s", by); ImGui::PopStyleColor();
                const char* site = "Powered by shifthub.uz";
                ImGui::SetCursorPos({ (W - ImGui::CalcTextSize(site).x) * 0.5f, H - 25 });
                ImGui::PushStyleColor(ImGuiCol_Text, { 0.18f, 0.30f, 0.24f, 1 });
                ImGui::Text("%s", site); ImGui::PopStyleColor();
            }
        }

        // ===============================================================
        //  DONE
        // ===============================================================
        else if (ePhase == EPhase::DONE)
        {
            ImGui::End();
            ImGui::Render();
            float cc[4] = { 0.04f, 0.05f, 0.07f, 1 };
            m_pContext->OMSetRenderTargets(1, &m_pRTV, NULL);
            m_pContext->ClearRenderTargetView(m_pRTV, cc);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            m_pSwapChain->Present(1, 0);
            break;
        }

        ImGui::End();
        ImGui::Render();
        float cc[4] = { 0.04f, 0.05f, 0.07f, 1 };
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
