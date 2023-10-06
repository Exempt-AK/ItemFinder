#pragma once
#include <d3d9.h>

namespace gui
{
	extern bool showMenu;
	extern bool setup;

	extern HWND window;
	extern WNDPROC origWindowProc;
	
	void SetupMenu(LPDIRECT3DDEVICE9 device);
	void DestroyMenu();
	void Render();
}