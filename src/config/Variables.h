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
		// V2.0 Aimbot rejimi: 0 = Klassik (eski, tez), 1 = Xavfsiz (anti-ban)
		CONFIG_ADD_VARIABLE(int,   m_iAimMode,          1);      // 0=Klassik, 1=Xavfsiz
		// V2.0 Anti-Ban (faqat Xavfsiz rejimda ishlaydi)
		CONFIG_ADD_VARIABLE(bool,  m_bVisibilityCheck, true);   // Devordan aim QILMASIN
		CONFIG_ADD_VARIABLE(float, m_flReactionTimeMin, 80.0f);  // Min reaktsiya vaqti (ms)
		CONFIG_ADD_VARIABLE(float, m_flReactionTimeMax, 200.0f); // Max reaktsiya vaqti (ms)
		CONFIG_ADD_VARIABLE(float, m_flMaxAimTime,     2500.0f); // Maks aim vaqti (ms)
		CONFIG_ADD_VARIABLE(float, m_flAimJitter,      1.5f);    // Tasodifiy xatolik (piksel)
		CONFIG_ADD_VARIABLE(float, m_flKillDelay,      400.0f);  // Target o'lganda kutish (ms)
	}; AimBotVariables_t m_AimBot;

	// =================== TRIGGERBOT ===================
	struct TriggerBotVariables_t
	{
		CONFIG_ADD_VARIABLE(bool,  m_bEnableTriggerbot, false);
		CONFIG_ADD_VARIABLE(bool,  m_bAutoShoot,        false); // Use checkbox for Auto Mode instead of key 0
		CONFIG_ADD_VARIABLE(int,   m_iTriggerKey,       0);     
		CONFIG_ADD_VARIABLE(float, m_flShotDelay,       50.0f);
		CONFIG_ADD_VARIABLE(bool,  m_bIgnoreTeammates,  true);
		CONFIG_ADD_VARIABLE(bool,  m_bOnlyVisible,      true); // don't shoot through walls
		// V2.0 Anti-Ban
		CONFIG_ADD_VARIABLE(float, m_flHitchance,       80.0f);  // 0-100% otish ehtimolligi
		CONFIG_ADD_VARIABLE(int,   m_iMinBurst,         1);      // Minimum burst o'qlari
		CONFIG_ADD_VARIABLE(int,   m_iMaxBurst,         1);      // Maximum burst o'qlari
	}; TriggerBotVariables_t m_TriggerBot;


	// ================ SPECTATOR LIST ==================
	struct SpectatorListVariables_t
	{
		CONFIG_ADD_VARIABLE(bool, m_bEnableSpectatorList, false);
	}; SpectatorListVariables_t m_SpectatorList;

	struct ESPVariables_t
	{
		CONFIG_ADD_VARIABLE(bool, m_bDroppedWeapons, false);
		CONFIG_ADD_VARIABLE(float, m_flWeaponDistance, 20.0f); // default 20 meters
	}; ESPVariables_t m_ESP;

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
		CONFIG_ADD_VARIABLE(bool, m_bDrawFilledBody, false);
		CONFIG_ADD_VARIABLE(bool, m_bDrawHeadDot,    false);
		CONFIG_ADD_VARIABLE(bool, m_bDrawSnaplines,  false);
		CONFIG_ADD_VARIABLE(bool, m_bDrawHasC4,      true);
		CONFIG_ADD_VARIABLE(bool, m_bDrawOffScreen,  true); // <-- Off-Screen ESP toggle

		CONFIG_ADD_VARIABLE(Color, m_colEnemyVisible,  Color(0,   255, 50,  255));
		CONFIG_ADD_VARIABLE(Color, m_colEnemyOccluded, Color(0,   180, 30,  200));
		CONFIG_ADD_VARIABLE(Color, m_colTeammate,      Color(50,  100, 255, 200));
	}; PlayerVisualsVariables_t m_PlayerVisuals;

	// ================== PLAYER GLOW ====================
	struct PlayerGlowVariables_t
	{
		CONFIG_ADD_VARIABLE(bool, m_bEnableGlow, false);
		CONFIG_ADD_VARIABLE(int,  m_iGlowType,   3);  // 0-none, 3=outer glow
		CONFIG_ADD_VARIABLE(Color, m_colGlowEnemy,    Color(255, 255, 255, 255)); // White glow
		CONFIG_ADD_VARIABLE(Color, m_colGlowTeam,     Color(50,  100, 255, 255)); // Blue glow
		CONFIG_ADD_VARIABLE(bool, m_bGlowEnemyOnly,   true);  // Only enemies
		CONFIG_ADD_VARIABLE(bool, m_bGlowInfo,        false); // HP and Weapon for glow
	}; PlayerGlowVariables_t m_PlayerGlow;

	// ===================== BHOP ======================
	struct BhopVariables_t
	{
		CONFIG_ADD_VARIABLE(bool, m_bEnableBhop, false);
		CONFIG_ADD_VARIABLE(int,  m_iBhopKey,    VK_XBUTTON1); // Mouse4 (5) — NOT Space!
		CONFIG_ADD_VARIABLE(bool, m_bEnableAutoStrafe, false); // Auto-Strafer
	}; BhopVariables_t m_Bhop;

	// ================== MISC =========================
	struct MiscVariables_t
	{
		CONFIG_ADD_VARIABLE(bool, m_bAntiFlash,       false);
		CONFIG_ADD_VARIABLE(bool, m_bC4Timer,         false);
		CONFIG_ADD_VARIABLE(bool, m_bGrenadeWarning,  false);
		CONFIG_ADD_VARIABLE(bool, m_bWatermark,       true);
		CONFIG_ADD_VARIABLE(bool, m_bSniperCrosshair, false);
		CONFIG_ADD_VARIABLE(bool, m_bHitSound,        false);
		CONFIG_ADD_VARIABLE(bool, m_bEnableSonar,     false);
		CONFIG_ADD_VARIABLE(float,m_flSonarFOV,       5.f);
		CONFIG_ADD_VARIABLE(bool, m_bAutoAccept,      true); // On by default globally
		// V2.0 Custom Sounds
		CONFIG_ADD_VARIABLE(bool,  m_bKillSound,       false);  // O'ldirganda alohida ovoz
		CONFIG_ADD_VARIABLE(float, m_flSoundVolume,    80.0f);  // Ovoz balandligi (0-100)
	}; MiscVariables_t m_Misc;

	// ================== RCS ==========================
	struct RCSVariables_t
	{
		CONFIG_ADD_VARIABLE(bool,  m_bEnable,         false);
		CONFIG_ADD_VARIABLE(float, m_flScaleX,        1.0f);
		CONFIG_ADD_VARIABLE(float, m_flScaleY,        1.0f);
	}; RCSVariables_t m_RCS;

	// ================ GRENADE HELPER ==================
	struct GrenadeHelperVariables_t
	{
		CONFIG_ADD_VARIABLE(bool,  m_bEnable,       false);
		CONFIG_ADD_VARIABLE(bool,  m_bShowAll,      true);  // Show all lineups as world markers
		CONFIG_ADD_VARIABLE(float, m_flMaxDistance,  2000.f); // Max render distance for world markers
		CONFIG_ADD_VARIABLE(bool,  m_bShowSmoke,    true);
		CONFIG_ADD_VARIABLE(bool,  m_bShowFlash,    true);
		CONFIG_ADD_VARIABLE(bool,  m_bShowMolotov,  true);
		CONFIG_ADD_VARIABLE(bool,  m_bShowHE,       true);
	}; GrenadeHelperVariables_t m_GrenadeHelper;

	// ================== WORLD ========================
	struct WorldVariables_t
	{
		CONFIG_ADD_VARIABLE(bool,  m_bNightMode,      false);
		CONFIG_ADD_VARIABLE(float, m_flNightModeValue,0.2f);
		CONFIG_ADD_VARIABLE(bool,  m_bFOVChanger,     false);
		CONFIG_ADD_VARIABLE(int,   m_iFOV,            105);
	}; WorldVariables_t m_World;

	// ================== HOTKEYS =======================
	struct HotkeyVariables_t
	{
		CONFIG_ADD_VARIABLE(int, m_iESPToggleKey, VK_F1);
	}; HotkeyVariables_t m_Hotkeys;

	// ==================== RADAR =======================
	struct RadarVariables_t
	{
		CONFIG_ADD_VARIABLE(bool,  m_bEnableRadar,  false);
		CONFIG_ADD_VARIABLE(bool,  m_bInGameRadar,  false); // Hack the CS2 ingame radar
		CONFIG_ADD_VARIABLE(float, m_flRadarSize,   200.0f);
		CONFIG_ADD_VARIABLE(float, m_flRadarRange,  2000.0f);
		CONFIG_ADD_VARIABLE(float, m_flRadarX,      20.0f);
		CONFIG_ADD_VARIABLE(float, m_flRadarY,      20.0f);
		CONFIG_ADD_VARIABLE(bool,  m_bRadarRotate,  true);
	}; RadarVariables_t m_Radar;
};
inline CVariables g_Variables;
