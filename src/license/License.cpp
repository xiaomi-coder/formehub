#include "../Includes.h"
#include <json.hpp>
using json = nlohmann::json;

// -----------------------------------------------------------------------
// Console login UI
// -----------------------------------------------------------------------
void CLicense::Load()
{
    m_eTier   = ETier::LITE;
    m_strUser = "Guest";

    system("cls");

    // === SHIFTHUB BANNER ===
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << R"(
   _____ _    _ _____ ______ _______ _    _ _    _ ____
  / ____| |  | |_   _|  ____|__   __| |  | | |  | |  _ \
 | (___ | |__| | | | | |__     | |  | |__| | |  | | |_) |
  \___ \|  __  | | | |  __|    | |  |  __  | |  | |  _ <
  ____) | |  | |_| |_| |       | |  | |  | | |__| | |_) |
 |_____/|_|  |_|_____|_|       |_|  |_|  |_|\____/|____/
    )" << std::endl;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
    std::cout << "  >> Professional CS2 Software <<" << std::endl;
    std::cout << "  ================================" << std::endl << std::endl;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    // === LOGIN LOOP ===
    for (int attempt = 0; attempt < 5; attempt++)
    {
        std::string username, password;

        std::cout << "  Username: ";
        std::getline(std::cin, username);

        std::cout << "  Password: ";
        // Hide password input
        char ch;
        password.clear();
        while ((ch = _getch()) != '\r')
        {
            if (ch == '\b')
            {
                if (!password.empty())
                {
                    password.pop_back();
                    std::cout << "\b \b";
                }
            }
            else
            {
                password += ch;
                std::cout << '*';
            }
        }
        std::cout << std::endl;

        // Trim
        while (!username.empty() && (username.back() == ' ' || username.back() == '\n' || username.back() == '\r'))
            username.pop_back();
        while (!password.empty() && (password.back() == ' ' || password.back() == '\n' || password.back() == '\r'))
            password.pop_back();

        if (username.empty() || password.empty())
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
            std::cout << "  [!] Username va password bosh bo'lmasligi kerak!" << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            continue;
        }

        // === SEND LOGIN REQUEST ===
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
        std::cout << "  [*] Serverga ulanmoqda..." << std::endl;

        json jBody;
        jBody["username"] = username;
        jBody["password"] = password;

        Http::Response resp = Http::Post(m_strApiUrl + "/api/auth/login", jBody.dump());

        if (!resp.success || resp.body.empty())
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
            if (resp.statusCode == 0)
                std::cout << "  [X] Server bilan bog'lanib bo'lmadi! Internet tekshiring." << std::endl;
            else
            {
                try {
                    json jErr = json::parse(resp.body);
                    std::cout << "  [X] Xato: " << jErr.value("error", "Login xato") << std::endl;
                } catch (...) {
                    std::cout << "  [X] Login xato (kod: " << resp.statusCode << ")" << std::endl;
                }
            }
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            std::cout << std::endl;
            continue;
        }

        // === PARSE RESPONSE ===
        try
        {
            json jResp = json::parse(resp.body);

            m_strToken = jResp.value("token", "");
            m_strUser  = jResp["user"].value("username", username);

            std::string strTier = jResp["user"].value("tier", "free");
            if (strTier == "pro")       m_eTier = ETier::PRO;
            else if (strTier == "mid")  m_eTier = ETier::MID;
            else                        m_eTier = ETier::LITE;

            m_strExpiry = jResp["user"].value("expires_at", "N/A");

            // === SUCCESS ===
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            std::cout << std::endl;
            std::cout << "  ================================" << std::endl;
            std::cout << "  [+] Login muvaffaqiyatli!" << std::endl;
            std::cout << "  [+] User: " << m_strUser << std::endl;
            std::cout << "  [+] Tier: " << GetTierName() << std::endl;
            std::cout << "  [+] Expires: " << m_strExpiry << std::endl;
            std::cout << "  ================================" << std::endl;

            // Download dependencies
            DownloadDependencies();

            // License check
            CheckLicense();

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            std::cout << std::endl;
            std::cout << "  [*] CS2 ochilishini kutmoqda..." << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            return;
        }
        catch (const std::exception& e)
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
            std::cout << "  [X] Server javobi noto'g'ri: " << e.what() << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }
    }

    // All attempts failed
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
    std::cout << std::endl;
    std::cout << "  [X] 5 ta urinish tugadi. Dastur yopilmoqda..." << std::endl;
    Sleep(3000);
    exit(1);
}

// -----------------------------------------------------------------------
// Check license validity with server
// -----------------------------------------------------------------------
bool CLicense::CheckLicense()
{
    if (m_strToken.empty()) return false;

    Http::Response resp = Http::Get(m_strApiUrl + "/api/license/check", m_strToken);
    if (!resp.success) return false;

    try
    {
        json jResp = json::parse(resp.body);

        if (!jResp.value("valid", false))
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
            std::cout << "  [!] Litsenziya muddati tugagan!" << std::endl;
            m_eTier = ETier::LITE;  // downgrade to free
            return false;
        }

        // Update tier from server
        std::string strTier = jResp.value("tier", "free");
        if (strTier == "pro")       m_eTier = ETier::PRO;
        else if (strTier == "mid")  m_eTier = ETier::MID;
        else                        m_eTier = ETier::LITE;

        return true;
    }
    catch (...) { return false; }
}

// -----------------------------------------------------------------------
// Send heartbeat to track online time
// -----------------------------------------------------------------------
void CLicense::SendHeartbeat()
{
    if (m_strToken.empty()) return;
    Http::Post(m_strApiUrl + "/api/license/heartbeat", "{}", m_strToken);
}

// -----------------------------------------------------------------------
// Download weapon dependencies from server
// -----------------------------------------------------------------------
void CLicense::DownloadDependencies()
{
    if (m_strToken.empty()) return;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
    std::cout << "  [*] Fayllar tekshirilmoqda..." << std::endl;

    // Get list of weapon files from server
    Http::Response resp = Http::Get(m_strApiUrl + "/api/files/list", m_strToken);
    if (!resp.success || resp.body.empty()) return;

    try
    {
        json jResp = json::parse(resp.body);
        if (!jResp.contains("files")) return;

        // Create weapons directory next to exe
        char szExePath[MAX_PATH] = {};
        GetModuleFileNameA(nullptr, szExePath, MAX_PATH);
        std::filesystem::path weaponsDir = std::filesystem::path(szExePath).parent_path() / "weapons";
        std::filesystem::create_directories(weaponsDir);

        for (auto& jFile : jResp["files"])
        {
            std::string filename = jFile.value("name", "");
            if (filename.empty()) continue;

            std::filesystem::path filePath = weaponsDir / filename;

            // Skip if already exists
            if (std::filesystem::exists(filePath))
                continue;

            // Download file
            std::cout << "  [*] Yuklanmoqda: " << filename << std::endl;
            Http::Response fileResp = Http::Get(
                m_strApiUrl + "/api/files/weapons/" + filename, m_strToken);

            if (fileResp.success && !fileResp.body.empty())
            {
                std::ofstream ofs(filePath, std::ios::binary);
                ofs.write(fileResp.body.data(), fileResp.body.size());
                ofs.close();
                std::cout << "  [+] Yuklandi: " << filename << std::endl;
            }
        }

        std::cout << "  [+] Fayllar tayyor!" << std::endl;
    }
    catch (...) {}
}
