#include "../Includes.h"

// Key format:  SH-<8 chars>-M   →  MID tier
//              SH-<8 chars>-P   →  PRO tier
// Lite is always free, no key needed.

void CLicense::Load()
{
    // default: always LITE
    m_eTier   = ETier::LITE;
    m_strUser = "Guest";

    // find key.txt next to our exe
    char szPath[MAX_PATH] = {};
    GetModuleFileNameA(nullptr, szPath, MAX_PATH);

    std::filesystem::path keyPath = std::filesystem::path(szPath).parent_path() / "key.txt";

    if (!std::filesystem::exists(keyPath))
        return;

    std::ifstream file(keyPath);
    if (!file.is_open())
        return;

    std::string strKey, strUser;
    std::getline(file, strKey);

    // optional second line: username
    if (std::getline(file, strUser) && !strUser.empty())
        m_strUser = strUser;

    file.close();

    // Trim whitespace
    while (!strKey.empty() && (strKey.back() == '\n' || strKey.back() == '\r' || strKey.back() == ' '))
        strKey.pop_back();

    // Validate format: must start with "SH-" and be 12+ chars
    if (strKey.size() < 12 || strKey.substr(0, 3) != "SH-")
        return;

    char cTier = strKey.back();

    if (cTier == 'P' || cTier == 'p')
    {
        m_eTier     = ETier::PRO;
        m_strExpiry = "Licensed";
    }
    else if (cTier == 'M' || cTier == 'm')
    {
        m_eTier     = ETier::MID;
        m_strExpiry = "Licensed";
    }
}
