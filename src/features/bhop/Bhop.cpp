#include "../../Includes.h"

static constexpr std::uint32_t FL_ONGROUND = (1 << 0);

static void PressKey(WORD wVk)
{
    INPUT inp{};
    inp.type       = INPUT_KEYBOARD;
    inp.ki.wVk     = wVk;
    inp.ki.dwFlags = 0;
    SendInput(1, &inp, sizeof(INPUT));
}

static void ReleaseKey(WORD wVk)
{
    INPUT inp{};
    inp.type       = INPUT_KEYBOARD;
    inp.ki.wVk     = wVk;
    inp.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &inp, sizeof(INPUT));
}

void Bhop::Run()
{
    static bool bSpaceHeld   = false;
    static bool bWasOnGround = true;
    static int  iJumpCount   = 0;

    bool bActive  = CONFIG_GET(bool, g_Variables.m_Bhop.m_bEnableBhop);
    int  iBhopKey = CONFIG_GET(int, g_Variables.m_Bhop.m_iBhopKey);
    bool bKeyHeld = (GetAsyncKeyState(iBhopKey) & 0x8000) != 0;

    if (!bActive || !bKeyHeld)
    {
        if (bSpaceHeld) { ReleaseKey(VK_SPACE); bSpaceHeld = false; }
        bWasOnGround = true;
        iJumpCount   = 0;
        return;
    }

    C_CSPlayerPawn* pLocal = g_Globals.m_LocalPlayer.m_pPlayerPawn;
    if (!pLocal)
    {
        if (bSpaceHeld) { ReleaseKey(VK_SPACE); bSpaceHeld = false; }
        return;
    }

    std::uint32_t uFlags = pLocal->m_fFlags();
    bool bOnGround = (uFlags & FL_ONGROUND) != 0;

    if (bOnGround)
    {
        if (!bWasOnGround)
        {
            // LANDING FRAME: immediately press space to jump on next tick
            if (!bSpaceHeld) { PressKey(VK_SPACE); bSpaceHeld = true; }
            iJumpCount++;
        }
        else
        {
            // Still on ground from previous tick (first frame or velocity stamped)
            // Keep space held to ensure the jump registers
            if (!bSpaceHeld) { PressKey(VK_SPACE); bSpaceHeld = true; }
        }
    }
    else
    {
        // IN AIR: release space so the engine doesn't eat the next jump
        if (bSpaceHeld) { ReleaseKey(VK_SPACE); bSpaceHeld = false; }
    }

    bWasOnGround = bOnGround;
}
