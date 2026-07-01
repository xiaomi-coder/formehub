#include "Includes.h"

bool CInterfaces::Update()
{
	m_GlobalVars = g_Memory.ReadMemory<CGlobalVars>(g_Memory.ReadMemory<std::uintptr_t>(g_Globals.m_Offsets.m_uGlobalVars));
	m_CSGOInput = g_Memory.ReadMemory<CCSGOInput>(g_Memory.ReadMemory<std::uintptr_t>(g_Globals.m_Offsets.m_uCSGOInput));
	m_NetworkGameClient = g_Memory.ReadMemory<CNetWorkGameClient>(g_Memory.ReadMemory<std::uintptr_t>(g_Globals.m_Offsets.m_uNetworkGameClient));
	m_GameEntitySystem = g_Memory.ReadMemory<CGameEntitySystem>(g_Memory.ReadMemory<std::uintptr_t>(g_Globals.m_Offsets.m_uEntitySystem));
	return true;
}

bool CGlobals::Update()
{
	static std::once_flag flag;
	std::call_once(flag, []()
	{
		// Default latest fallback offsets (a2x/cs2-dumper — July 2026)
		std::uintptr_t uEntityList = 38696608;
		std::uintptr_t uViewMatrix = 36989744;
		std::uintptr_t uLocalPlayerController = 36833056;
		std::uintptr_t uPlantedC4 = 37027736;
		std::uintptr_t uGlobalVars = 33953488;
		std::uintptr_t uCSGOInput = 37052992;
		std::uintptr_t uNetworkGameClient = 9478560;
		std::uintptr_t uEntitySystem = 38696608;
		std::uintptr_t uSensitivity = 36956776;

		// Attempt to download the latest offsets dynamically
		std::cout << X("  [~] Downloading latest CS2 offsets...") << std::endl;
		// Use a2x/cs2-dumper as primary URL as it matches the live game build accurately
		Http::Response resp = Http::Get(X("https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/offsets.json"));
		if (!resp.success || resp.body.empty())
		{
			// Try secondary URL
			resp = Http::Get(X("https://raw.githubusercontent.com/sezzyaep/CS2-OFFSETS/main/offsets.json"));
		}

		if (resp.success && !resp.body.empty())
		{
			try
			{
				nlohmann::json jOffsets = nlohmann::json::parse(resp.body);
				if (jOffsets.contains(X("client.dll")))
				{
					auto& client = jOffsets[X("client.dll")];
					uEntityList = client.value(X("dwEntityList"), uEntityList);
					uViewMatrix = client.value(X("dwViewMatrix"), uViewMatrix);
					uLocalPlayerController = client.value(X("dwLocalPlayerController"), uLocalPlayerController);
					uPlantedC4 = client.value(X("dwPlantedC4"), uPlantedC4);
					uGlobalVars = client.value(X("dwGlobalVars"), uGlobalVars);
					uCSGOInput = client.value(X("dwCSGOInput"), uCSGOInput);
					uEntitySystem = client.value(X("dwGameEntitySystem"), uEntitySystem);
					uSensitivity = client.value(X("dwSensitivity"), uSensitivity);
					std::cout << X("  [+] Dynamic offsets downloaded successfully!") << std::endl;
				}
				if (jOffsets.contains(X("engine2.dll")))
				{
					auto& engine = jOffsets[X("engine2.dll")];
					uNetworkGameClient = engine.value(X("dwNetworkGameClient"), uNetworkGameClient);
				}
			}
			catch (const std::exception& ex)
			{
				std::cout << X("  [X] Failed to parse downloaded offsets: ") << ex.what() << std::endl;
			}
			catch (...)
			{
				std::cout << X("  [X] Unknown error parsing downloaded offsets.") << std::endl;
			}
		}
		else
		{
			std::cout << X("  [!] Failed to download offsets. Using latest hardcoded fallbacks.") << std::endl;
		}

		g_Globals.m_Offsets.m_uEntityList = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress + uEntityList;
		g_Globals.m_Offsets.m_uViewMatrix = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress + uViewMatrix;
		g_Globals.m_Offsets.m_uLocalPlayerController = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress + uLocalPlayerController;
		g_Globals.m_Offsets.m_uPlantedC4 = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress + uPlantedC4;
		g_Globals.m_Offsets.m_uAutoAcceptArray = g_Memory.PatternScan(CLIENT_DLL, X("48 89 05 ? ? ? ? E8 ? ? ? ? 48 85 DB"), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7);
		
		g_Globals.m_Offsets.m_uGlobalVars = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress + uGlobalVars;
		g_Globals.m_Offsets.m_uCSGOInput = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress + uCSGOInput;
		g_Globals.m_Offsets.m_uNetworkGameClient = g_Memory.GetModule(ENGINE2_DLL).m_uBaseAddress + uNetworkGameClient;
		g_Globals.m_Offsets.m_uEntitySystem = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress + uEntitySystem;
		g_Globals.m_Offsets.m_uSensitivity = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress + uSensitivity;
	});

	g_Globals.m_uEntityList = g_Globals.m_Offsets.m_uEntityList;
	g_Globals.m_LocalPlayer.m_pController = g_Memory.ReadMemory<CCSPlayerController*>(g_Globals.m_Offsets.m_uLocalPlayerController);
	if (g_Globals.m_LocalPlayer.m_pController)
		g_Globals.m_LocalPlayer.m_pPlayerPawn = reinterpret_cast<C_CSPlayerPawn*>(g_Globals.m_LocalPlayer.m_pController->m_hPawn().Get());
	else
		g_Globals.m_LocalPlayer.m_pPlayerPawn = nullptr;
	g_Globals.m_matViewMatrix = g_Memory.ReadMemory<ViewMatrix_t>(g_Globals.m_Offsets.m_uViewMatrix);

	return true;
}