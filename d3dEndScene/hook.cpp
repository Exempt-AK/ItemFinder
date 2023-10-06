#include "includes.h"

void Patch(BYTE* src, BYTE* dst, unsigned int len)
{
	DWORD oProc;
	VirtualProtect(dst, len, PAGE_EXECUTE_READWRITE, &oProc);
	memcpy(dst, src, len);
	VirtualProtect(dst, len, oProc, &oProc);
}

bool Hook(char* src, char* dst, unsigned int len)
{
	if (len < 5) return false;
	DWORD oProc;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &oProc);
	memset(src, 0x90, len);
	uintptr_t relAddy = (uintptr_t)(dst - src - 5);
	*src = (char)0xE9;
	*(uintptr_t*)(src + 1) = (uintptr_t)relAddy;
	VirtualProtect(src, len, oProc, &oProc);

	return true;
}

char* TrampHook(char* src, char* dst, unsigned int len)
{
	if (len < 5) return 0;
	char* gateway = (char*)VirtualAlloc(0, len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!gateway) return nullptr;
	memcpy(gateway, src, len);
	uintptr_t jumpAddy = (uintptr_t)(src - gateway - 5);
	*(gateway + len) = (char)0xE9;
	*(uintptr_t*)(gateway + len + 1) = jumpAddy;
	if (Hook(src, dst, len))
	{
		return gateway;
	}
	else
		return nullptr;

}