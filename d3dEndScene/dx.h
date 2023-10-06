#pragma once

typedef HRESULT(APIENTRY* tEndScene) (LPDIRECT3DDEVICE9 pDevice);
typedef HRESULT(APIENTRY* tReset)(void*, LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pParams);

bool GetD3D9Device(void** pTable, size_t size);

static HWND window;

HWND GetProcessWindow();

extern int windowHeight, windowWidth;

extern LPDIRECT3DDEVICE9 pDevice;