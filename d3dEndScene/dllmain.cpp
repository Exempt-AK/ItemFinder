#pragma once
#include "includes.h"

std::chrono::high_resolution_clock::time_point startTime;
std::chrono::high_resolution_clock::time_point endTime;
double frameTime = 0.0;

bool fontCreated = false;
LPD3DXFONT	m_font = NULL;

BYTE EndSceneBytes[7]{ 0 };
tEndScene oEndScene = nullptr;

BYTE ResetBytes[7]{ 0 };
tReset oReset = nullptr;

uintptr_t mouseAddr = 0x0056858;

void APIENTRY hkEndScene(LPDIRECT3DDEVICE9 o_pDevice)
{
	if (!pDevice)
		pDevice = o_pDevice;

	if (!fontCreated)
	{
		D3DXCreateFont(pDevice, 25, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_font);
		fontCreated = true;
	}

	/*if (!gui::setup)
		gui::SetupMenu(pDevice);
	gui::Render();*/

	//Draw.
	for (auto& pos : drawPositions)
	{
		D3DXVECTOR3 vec = WorldToScreen2(pDevice, playerX, playerY, pos.first.x, pos.first.y);
		DrawMessage(m_font, vec.x, vec.y, 255, 255, 0, 255, pos.second.name.c_str());
		//DrawFilledRect(pos.first.x, pos.first.y, 25, 25, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	oEndScene(pDevice);
}

HRESULT APIENTRY hkReset(void* thisPtr, LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pParams)
{
	//ImGui_ImplDX9_InvalidateDeviceObjects();
	const auto result = oReset(thisPtr, pDevice, pParams);
	//ImGui_ImplDX9_CreateDeviceObjects();
	return result;
}

DWORD WINAPI MainLoop(HMODULE hModule)
{
	AllocConsole();
	FILE* file;
	freopen_s(&file, "CONOUT$", "w", stdout);

	ParseItemConfig();

	void* d3dDevice[119];

	HANDLE hProcess = GetCurrentProcess();

	GetUpdateDataMemoryAddress(hProcess);

	if (GetD3D9Device(d3dDevice, sizeof(d3dDevice)))
	{
		memcpy(EndSceneBytes, (char*)d3dDevice[42], 7);
		oEndScene = (tEndScene)TrampHook((char*)d3dDevice[42], (char*)hkEndScene, 7);

		memcpy(ResetBytes, (char*)d3dDevice[16], 7);
		oReset = (tReset)TrampHook((char*)d3dDevice[16], (char*)hkReset, 7);
	}

	while (!GetAsyncKeyState(VK_END))
	{
		startTime = std::chrono::high_resolution_clock::now();

		UpdateWorldData(hProcess);
		
		Sleep(3);

		endTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = endTime - startTime;
		frameTime = duration.count();
	}

	if (file)
		fclose(file);
	FreeConsole();

	//gui::DestroyMenu();

	m_font->Release();
	pDevice->Release();
	Patch(EndSceneBytes, (BYTE*)d3dDevice[42], 7);
	Patch(ResetBytes, (BYTE*)d3dDevice[16], 7);
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpr)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainLoop, hModule, 0, 0));
	}
	return TRUE;
}