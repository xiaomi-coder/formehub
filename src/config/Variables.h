#pragma once

enum EVisualsModifiers : std::uint8_t
{
	VISUALS_IGNORE_TEAMMATES = 0U,
	VISUALS_ONLY_WHEN_VISIBLE,
	VISUALS_MAX
};

enum EBoxType : int
{
	BOX_TYPE_2D = 0,
	BOX_TYPE_CORNER,
	BOX_TYPE_BOTH
};

class CVariables
{
public:
	struct GuiVariables_t
	{
		CONFIG_ADD_VARIABLE(int, m_iMenuKey, VK_INSERT);
		CONFIG_ADD_VARIABLE(int, m_iUnloadKey, VK_DELETE);
		CONFIG_ADD_VARIABLE(bool, m_bExcludeFromDesktopCapture, false);
	}; GuiVariables_t m_Gui;

	// ===================== AIMBOT =====================
	struct AimBotVariables_t
	{
		CONFIG_ADD_VARIABLE(bool,  m_bEnableAimbot,    false);
		CONFIG_ADD_VARIABLE(int,   m_iAimKey,          VK_LBUTTON);
		CONFIG_ADD_VARIABLE(float, m_flFOV,            5.0f);
		CONFIG_ADD_VARIABLE(float, m_flSmooth,         5.0f);
		CONFIG_ADD_VARIABLE(int,   m_iHitbox,          0);
		CONFIG_ADD_VARIABLE(bool,  m_bIgnoreTeammates, true);
		CONFIG_ADD_VARIABLE(bool,  m_bSilentAim,       false);
		CONFIG_ADD_VARIABLE(bool,  m_bDrawFOV,         true);
	}; AimBotVariables_t m_AimBot;

	// =================== TRIGGERBOT ===================
	struct TriggerBotVariables_t
	{
		CONFIG_ADD_VARIABLE(bool,  m_bEnableTriggerbot, false);
		CONFIG_ADD_VARIABLE(int,   m_iTriggerKey,       0); // 0 = auto (no hold key)
		CONFIG_ADD_VARIABLE(float, m_flShotDelay,       50.0f);
		CONFIG_ADD_VARIABLE(bool,  m_bIgnoreTeammates,  true);
		CONFIG_ADD_VARIABLE(bool,  m_bOnlyVisible,      true); // don't shoot through walls
	}; TriggerBotVariables_t m_TriggerBot;

	// ====================== ESP =======================
	struct PlayerVisualsVariables_t
	{
		CONFIG_ADD_VARIABLE(bool, m_bEnableVisuals, false);
		CONFIG_ADD_VARIABLE_VECTOR(bool, EVisualsModifiers::VISUALS_MAX, m_vecVisualsModifiers, false);

		CONFIG_ADD_VARIABLE(bool, m_bDrawBox,        true);
		CONFIG_ADD_VARIABLE(int,  m_iBoxType,        BOX_TYPE_BOTH);
		CONFIG_ADD_VARIABLE(bool, m_bDrawBoxOutline, true);
		CONFIG_ADD_VARIABLE(bool, m_bDrawHealthBar,  true);
		CONFIG_ADD_VARIABLE(bool, m_bDrawName,       true);
		CONFIG_ADD_VARIABLE(bool, m_bDrawWeapon,     true);
		CONFIG_ADD_VARIABLE(bool, m_bDrawDistance,   false);
		CONFIG_ADD_VARIABLE(bool, m_bDrawSkeleton,   false);
		CONFIG_ADD_VARIABLE(bool, m_bDrawHeadDot,    false);
		CONFIG_ADD_VARIABLE(bool, m_bDrawSnaplines,  false);

		CONFIG_ADD_VARIABLE(Color, m_colEnemyVisible,  Color(0,   255, 50,  255));
		CONFIG_ADD_VARIABLE(Color, m_colEnemyOccluded, Color(0,   180, 30,  200));
		CONFIG_ADD_VARIABLE(Color, m_colTeammate,      Color(50,  100, 255, 200));
	}; PlayerVisualsVariables_t m_PlayerVisuals;

	// ===================== BHOP ======================
	struct BhopVariables_t
	{
		CONFIG_ADD_VARIABLE(bool, m_bEnableBhop, false);
		CONFIG_ADD_VARIABLE(int,  m_iBhopKey,    VK_XBUTTON1); // Mouse4 (5) — NOT Space!
	}; BhopVariables_t m_Bhop;

	// ================== MISC =========================
	struct MiscVariables_t
	{
		CONFIG_ADD_VARIABLE(bool, m_bAntiFlash,    false);
		CONFIG_ADD_VARIABLE(bool, m_bC4Timer,      false);
	}; MiscVariables_t m_Misc;

	// ================== HOTKEYS =======================
	struct HotkeyVariables_t
	{
		CONFIG_ADD_VARIABLE(int, m_iESPToggleKey, VK_F1);
	}; HotkeyVariables_t m_Hotkeys;

	// ==================== RADAR =======================
	struct RadarVariables_t
	{
		CONFIG_ADD_VARIABLE(bool,  m_bEnableRadar,  false);
		CONFIG_ADD_VARIABLE(float, m_flRadarSize,   200.0f);
		CONFIG_ADD_VARIABLE(float, m_flRadarRange,  2000.0f);
		CONFIG_ADD_VARIABLE(float, m_flRadarX,      20.0f);
		CONFIG_ADD_VARIABLE(float, m_flRadarY,      20.0f);
		CONFIG_ADD_VARIABLE(bool,  m_bRadarRotate,  true);
	}; RadarVariables_t m_Radar;
};
inline CVariables g_Variables;
