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
		g_Globals.m_Offsets.m_uEntityList = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress + 38620800;
		g_Globals.m_Offsets.m_uViewMatrix = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress + 36915280;
		g_Globals.m_Offsets.m_uLocalPlayerController = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress + 36758976;
		g_Globals.m_Offsets.m_uPlantedC4 = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress + 36950376;
		g_Globals.m_Offsets.m_uAutoAcceptArray = g_Memory.PatternScan(CLIENT_DLL, X("48 89 05 ? ? ? ? E8 ? ? ? ? 48 85 DB"), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7);
		
		g_Globals.m_Offsets.m_uGlobalVars = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress + 33879552;
		g_Globals.m_Offsets.m_uCSGOInput = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress + 36978288;
		g_Globals.m_Offsets.m_uNetworkGameClient = g_Memory.GetModule(ENGINE2_DLL).m_uBaseAddress + 9478560;
		g_Globals.m_Offsets.m_uEntitySystem = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress + 38620800;
		g_Globals.m_Offsets.m_uSensitivity = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress + 36882696;
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