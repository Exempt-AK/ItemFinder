#include "includes.h"

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx9.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProcess(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool gui::showMenu = true;
bool gui::setup = false;
HWND gui::window = nullptr;
WNDPROC gui::origWindowProc = nullptr;


void gui::SetupMenu(LPDIRECT3DDEVICE9 device)
{
	auto params = D3DDEVICE_CREATION_PARAMETERS{};
	device->GetCreationParameters(&params);
	window = params.hFocusWindow;

	origWindowProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProcess)));

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	if (!ImGui_ImplWin32_Init(window))
		printf("Failed at ImGui_ImplWin32_Init\n");
	
	if (!ImGui_ImplDX9_Init(device))
		printf("Failed at ImGui_ImplDX9_Init\n");

	setup = true;
}

void gui::DestroyMenu()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(origWindowProc));
}

void gui::Render()
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow(&showMenu);
	if (!ImGui::Begin("Menu...", &showMenu))
		printf("Failed at Begin\n");
	ImGui::End();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

LRESULT CALLBACK WindowProcess(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_F10) & 1)
	{
		gui::showMenu = !gui::showMenu;
		printf("showMenu: %i", gui::showMenu);
	}

	if (gui::showMenu && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return 1L;

	return CallWindowProc(gui::origWindowProc, hWnd, msg, wParam, lParam);
}