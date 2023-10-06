#include <iostream>
#include <windows.h>
#include <string>
#include <thread>
#include <TlHelp32.h>
#include <Shellapi.h>
#include <Shlwapi.h>

using namespace std;

DWORD GetProcId(const char* procName)
{
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_stricmp(procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
				}

			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return procId;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	FreeConsole();

	char moduleFileName[MAX_PATH];
	if (GetModuleFileName(NULL, moduleFileName, MAX_PATH) == 0) 
	{
		std::cerr << "Error getting module file name." << std::endl;
		return 1;
	}

	// Extract the directory from the module file name
	std::string modulePath = moduleFileName;
	size_t lastSlash = modulePath.find_last_of('\\');
	std::string directory = modulePath.substr(0, lastSlash + 1);

	// Construct the full path to Core.dll using the relative path
	std::string coreDllPath = directory + "Core.dll";

	const char* dllPath = coreDllPath.c_str();
	const char* procName = "Xenimus2.exe";
	DWORD procId = 0;

	STARTUPINFO startInfo = { 0 };
	PROCESS_INFORMATION procInfo = { 0 };
	CreateProcess(TEXT("Xenimus2.exe"), NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &startInfo, &procInfo);

	while (!procId)
	{
		procId = GetProcId(procName);
		printf("%i\n", procId);
		Sleep(50);
	}

	/*HWND hwnd = FindWindow(NULL, "Xenimus");
	if (hwnd)
	{
		SetWindowPos(hwnd, NULL, 0, 0, 1400, 1200, SWP_SHOWWINDOW);
	}*/

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
	if (hProc && hProc != INVALID_HANDLE_VALUE)
	{
		printf("hProc is valid.\n");
		void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		printf("Path location %p\n", loc);

		if (loc)
			if (WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0))
				printf("Path wrote to memory.\n");

		HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);

		if (hThread)
		{
			printf("Thread started and handle closed.\n");
			WaitForSingleObject(hThread, INFINITE);
			CloseHandle(hThread);
		}
		if (hProc)
			CloseHandle(hProc);
	}

	return 0;
}