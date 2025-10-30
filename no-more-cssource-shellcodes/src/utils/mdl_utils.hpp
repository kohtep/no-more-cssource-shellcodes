#pragma once

#include <Windows.h>
#include <stdint.h>

struct module_t;

struct sighandle_t
{
	module_t *Module;
	void *Ptr;

	sighandle_t(module_t *mdl);

	bool IsValid();

	sighandle_t &Reset();
	sighandle_t &Invalidate();

	sighandle_t &FindU8(uint8_t value, bool backward = false, ptrdiff_t offset = 0);
	sighandle_t &FindU16(uint16_t value, bool backward = false, ptrdiff_t offset = 0);
	sighandle_t &FindU24(uint8_t value[3], bool backward = false, ptrdiff_t offset = 0);
	sighandle_t &FindU24(uint32_t value, bool backward = false, ptrdiff_t offset = 0);
	sighandle_t &FindU32(uint32_t value, bool backward = false, ptrdiff_t offset = 0);

	bool CheckU8(uint8_t value, ptrdiff_t offset = 0);
	bool CheckU16(uint16_t value, ptrdiff_t offset = 0);
	bool CheckU24(uint32_t value, ptrdiff_t offset = 0);
	bool CheckU24(const uint8_t value[3], ptrdiff_t offset = 0);
	bool CheckU32(uint32_t value, ptrdiff_t offset = 0);

	sighandle_t &Resolve();
	sighandle_t &Rip();
};

struct module_t
{
	HMODULE Handle = nullptr;
	DWORD Size = 0;
	void *LastByte = nullptr;

	module_t() = default;
	module_t(const char *libname);

	sighandle_t Sig();
};