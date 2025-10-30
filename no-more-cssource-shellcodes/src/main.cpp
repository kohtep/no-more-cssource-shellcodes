#include <Windows.h>

#include "common/logger.hpp"

#include "core/globals.hpp"
#include "core/hooks.hpp"

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		InitGlobals();

	return TRUE;
}