#pragma once

namespace InternalCore
{
    // Worker thread body. Never returns — exits via FreeLibraryAndExitThread.
    void Run(HMODULE hSelf);
}
