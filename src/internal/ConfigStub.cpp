#include "../Includes.h"

// -----------------------------------------------------------------------
// Loader-lock-safe replacement for Config::GetWorkingPath().
//
// This module never reads or writes configs, but Config.h declares
// file-scope path constants that call GetWorkingPath() during STATIC
// INITIALIZATION — which, for an injected DLL, runs while the Windows
// loader lock is held.
//
// The real implementation (src/config/Config.cpp) uses SHGetKnownFolderPath;
// shell APIs may load further DLLs, and loading a DLL under the loader lock
// can deadlock the game. So the internal build links this version instead
// and resolves the path from the environment, which touches only kernel32
// (already loaded, no additional DLL loads).
// -----------------------------------------------------------------------

std::filesystem::path Config::GetWorkingPath()
{
    char szProfile[MAX_PATH]{};
    if (GetEnvironmentVariableA("USERPROFILE", szProfile, MAX_PATH) > 0)
        return std::filesystem::path(szProfile) / "Documents" / "Base";

    return std::filesystem::path();
}
