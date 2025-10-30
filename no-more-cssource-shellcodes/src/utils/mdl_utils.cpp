#include "mdl_utils.hpp"

#include "utils/mem_utils.hpp"

static DWORD GetModuleSize(HMODULE handle)
{
	if (!handle)
		return 0;

	PIMAGE_DOS_HEADER dos = reinterpret_cast<PIMAGE_DOS_HEADER>(handle);
	PIMAGE_NT_HEADERS nt = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uintptr_t>(dos) + dos->e_lfanew);

	return nt->OptionalHeader.SizeOfImage;
}

bool IsMemoryValid(void *pAddress)
{
	MEMORY_BASIC_INFORMATION memInfo;

	if (VirtualQuery(pAddress, &memInfo, sizeof(memInfo)) == 0)
		return false;

	return !(memInfo.Protect == 0 || memInfo.Protect == PAGE_NOACCESS);
}

sighandle_t::sighandle_t(module_t *mdl) : Module(mdl)
{
	Ptr = mdl->Handle;
}

bool sighandle_t::IsValid()
{
	if (!Module || !Ptr)
		return false;

	return IsMemoryValid(Ptr);
}

sighandle_t &sighandle_t::Reset()
{
	Ptr = Module->Handle;
	return *this;
}

sighandle_t &sighandle_t::Invalidate()
{
	Ptr = nullptr;
	return *this;
}

sighandle_t &sighandle_t::FindU8(uint8_t value, bool backward, ptrdiff_t offset)
{
	Ptr = U::Memory::FindU8(Ptr, Module->Handle, Module->LastByte, value, backward, offset);
	return *this;
}

sighandle_t &sighandle_t::FindU16(uint16_t value, bool backward, ptrdiff_t offset)
{
	Ptr = U::Memory::FindU16(Ptr, Module->Handle, Module->LastByte, value, backward, offset);
	return *this;
}

sighandle_t &sighandle_t::FindU24(uint8_t value[3], bool backward, ptrdiff_t offset)
{
	Ptr = U::Memory::FindU24(Ptr, Module->Handle, Module->LastByte, value, backward, offset);
	return *this;
}

sighandle_t &sighandle_t::FindU24(uint32_t value, bool backward, ptrdiff_t offset)
{
	Ptr = U::Memory::FindU24(Ptr, Module->Handle, Module->LastByte, value, backward, offset);
	return *this;
}

sighandle_t &sighandle_t::FindU32(uint32_t value, bool backward, ptrdiff_t offset)
{
	Ptr = U::Memory::FindU32(Ptr, Module->Handle, Module->LastByte, value, backward, offset);
	return *this;
}

bool sighandle_t::CheckU8(uint8_t value, ptrdiff_t offset)
{
	return U::Memory::CheckU8(Ptr, value, offset);
}

bool sighandle_t::CheckU16(uint16_t value, ptrdiff_t offset)
{
	return U::Memory::CheckU16(Ptr, value, offset);
}

bool sighandle_t::CheckU24(uint32_t value, ptrdiff_t offset)
{
	return U::Memory::CheckU24(Ptr, value, offset);
}

bool sighandle_t::CheckU24(const uint8_t value[3], ptrdiff_t offset)
{
	return U::Memory::CheckU24(Ptr, value, offset);
}

bool sighandle_t::CheckU32(uint32_t value, ptrdiff_t offset)
{
	return U::Memory::CheckU32(Ptr, value, offset);
}

sighandle_t &sighandle_t::Resolve()
{
	Ptr = U::Memory::Resolve(Ptr, 0, sizeof(uint32_t));
	return *this;
}

sighandle_t &sighandle_t::Rip()
{
	if (!Ptr)
		return *this;

	Ptr = *reinterpret_cast<void **>(Ptr);
	return *this;
}

module_t::module_t(const char *libname)
{
	Handle = GetModuleHandleA(libname);
	Size = GetModuleSize(Handle);
	LastByte = U::Memory::PtrOffset(Handle, Size - 1);
}

sighandle_t module_t::Sig()
{
	return sighandle_t(this);
}