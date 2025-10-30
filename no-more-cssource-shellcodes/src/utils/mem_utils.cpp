#include "mem_utils.hpp"

#include <assert.h>
#include <Windows.h>
#include <cstring>

using FindMemoryCmp_t = bool(*)(const void *addr1, const void *addr2, size_t size, void *param);

#define MEM_NAMESPACE_BEGIN namespace U::Memory {
#define MEM_NAMESPACE_END }

MEM_NAMESPACE_BEGIN

static bool IsInBounds(uintptr_t addr, uintptr_t addr_lower, uintptr_t addr_upper)
{
	if (addr < addr_lower)
		return false;

	if (addr >= addr_upper)
		return false;

	return true;
}

static bool IsInBounds(const void *addr, const void *addr_lower, const void *addr_upper)
{
	auto naddr = reinterpret_cast<uintptr_t>(addr);
	auto lower = reinterpret_cast<uintptr_t>(addr_lower);
	auto upper = reinterpret_cast<uintptr_t>(addr_upper);

	return IsInBounds(naddr, lower, upper);
}

static bool FindMemoryCmp(const void *addr1, const void *addr2, size_t size, void *param)
{
	return memcmp(addr1, addr2, size) == 0;
}

static bool IsMemoryExecutable(const void *addr, ptrdiff_t offset)
{
	if (offset != 0)
		addr = (void *)(uintptr_t(addr) + offset);

	if (!addr)
		return false;

	MEMORY_BASIC_INFORMATION memInfo;

	if (VirtualQuery(addr, &memInfo, sizeof(memInfo)) == 0)
		return false;

	if (memInfo.Protect == 0 || memInfo.Protect == PAGE_NOACCESS)
		return false;

	return memInfo.Protect == PAGE_EXECUTE ||
		memInfo.Protect == PAGE_EXECUTE_READ ||
		memInfo.Protect == PAGE_EXECUTE_READWRITE ||
		memInfo.Protect == PAGE_EXECUTE_WRITECOPY;
}

static bool WriteMemory(void *addr, const void *data, size_t size, ptrdiff_t offset, bool use_setmem = false)
{
	if (offset != 0)
		addr = PtrOffset(addr, offset);

	DWORD new_protection, old_protection;

	if (IsMemoryExecutable(addr, offset))
		new_protection = PAGE_EXECUTE_READWRITE;
	else
		new_protection = PAGE_READWRITE;

	if (!VirtualProtect(addr, size, new_protection, &old_protection))
		return false;

	if (use_setmem)
	{
		memset(addr, static_cast<int>(*static_cast<const uint8_t *>(data)), size);
	}
	else
	{
		memcpy(addr, data, size);
	}

	if (!VirtualProtect(addr, size, old_protection, &old_protection))
		return false;

	return true;
}

static bool CheckMemory(const void *addr, const void *value, size_t size, ptrdiff_t offset)
{
	if (!addr)
		return false;

	return memcmp(PtrOffset(addr, offset), value, size) == 0;
}

void *PtrOffset(const void *addr, ptrdiff_t offset, bool dereference)
{
	void *result = reinterpret_cast<void *>(reinterpret_cast<intptr_t>(addr) + offset);

	if (dereference)
		result = *reinterpret_cast<void **>(result);

	return result;
}

void *FindMemory(const void *addr_start, const void *addr_min, const void *addr_max, const void *data, size_t size, bool backward,
	ptrdiff_t offset = 0, FindMemoryCmp_t comparator = FindMemoryCmp, void *comparator_param = nullptr)
{
	assert(addr_min != nullptr && addr_max != nullptr && addr_min <= addr_max);

	if (!comparator)
		comparator = FindMemoryCmp;

	const void *result = static_cast<const void *>(addr_start);
	addr_max = reinterpret_cast<const void *>(reinterpret_cast<intptr_t>(addr_max) - size);

	do
	{
		if (!IsInBounds(result, addr_min, addr_max))
			return nullptr;

		if (comparator(result, data, size, comparator_param))
			return reinterpret_cast<void *>(uintptr_t(result) + offset);

		if (backward)
			result = reinterpret_cast<void *>(uintptr_t(result) - 1);
		else
			result = reinterpret_cast<void *>(uintptr_t(result) + 1);

	} while (true);

	return nullptr;
}

uint8_t *FindU8(const void *addr_start, const void *addr_min, const void *addr_max, uint8_t value, bool backward, ptrdiff_t offset)
{
	return static_cast<uint8_t *>(FindMemory(addr_start, addr_min, addr_max, &value, sizeof(value), backward, offset));
}

uint16_t *FindU16(const void *addr_start, const void *addr_min, const void *addr_max, uint16_t value, bool backward, ptrdiff_t offset)
{
	return static_cast<uint16_t *>(FindMemory(addr_start, addr_min, addr_max, &value, sizeof(value), backward, offset));
}

uint32_t *FindU24(const void *addr_start, const void *addr_min, const void *addr_max, uint8_t value[3], bool backward, ptrdiff_t offset)
{
	return static_cast<uint32_t *>(FindMemory(addr_start, addr_min, addr_max, &value, 3, backward, offset));
}

uint32_t *FindU24(const void *addr_start, const void *addr_min, const void *addr_max, uint32_t value, bool backward, ptrdiff_t offset)
{
	uint8_t bytes[3] =
	{
		static_cast<uint8_t>(value & 0xFF),
		static_cast<uint8_t>((value >> 8) & 0xFF),
		static_cast<uint8_t>((value >> 16) & 0xFF)
	};

	return static_cast<uint32_t *>(FindMemory(addr_start, addr_min, addr_max, &bytes, 3, backward, offset));
}

uint32_t *FindU32(const void *addr_start, const void *addr_min, const void *addr_max, uint32_t value, bool backward, ptrdiff_t offset)
{
	return static_cast<uint32_t *>(FindMemory(addr_start, addr_min, addr_max, &value, sizeof(value), backward, offset));
}

bool WriteU8(void *addr, uint8_t value, ptrdiff_t offset)
{
	return WriteMemory(addr, &value, sizeof(value), offset);
}

bool WriteU16(void *addr, uint16_t value, ptrdiff_t offset)
{
	return WriteMemory(addr, &value, sizeof(value), offset);
}

bool WriteU24(void *addr, const int8_t value[3], ptrdiff_t offset)
{
	return WriteMemory(addr, value, 3, offset);
}

bool WriteU24(void *addr, uint32_t value, ptrdiff_t offset)
{
	uint8_t bytes[3] =
	{
		static_cast<uint8_t>(value & 0xFF),
		static_cast<uint8_t>((value >> 8) & 0xFF),
		static_cast<uint8_t>((value >> 16) & 0xFF)
	};

	return WriteMemory(addr, &bytes, sizeof(bytes), offset);
}

bool WriteU32(void *addr, uint32_t value, ptrdiff_t offset)
{
	return WriteMemory(addr, &value, sizeof(value), offset);
}

bool WritePointer(void *addr, const void *value, ptrdiff_t offset)
{
	return WriteMemory(addr, &value, sizeof(value), offset);
}

bool CheckU8(const void *addr, uint8_t value, ptrdiff_t offset)
{
	return CheckMemory(addr, &value, sizeof(value), offset);
}

bool CheckU16(const void *addr, uint16_t value, ptrdiff_t offset)
{
	return CheckMemory(addr, &value, sizeof(value), offset);
}

bool CheckU24(const void *addr, uint32_t value, ptrdiff_t offset)
{
	uint8_t bytes[3] =
	{
		static_cast<uint8_t>(value & 0xFF),
		static_cast<uint8_t>((value >> 8) & 0xFF),
		static_cast<uint8_t>((value >> 16) & 0xFF)
	};

	return CheckMemory(addr, &bytes, 3, offset);
}

bool CheckU24(const void *addr, const uint8_t value[3], ptrdiff_t offset)
{
	return CheckMemory(addr, value, 3, offset);
}

bool CheckU32(const void *addr, uint32_t value, ptrdiff_t offset)
{
	return CheckMemory(addr, &value, sizeof(value), offset);
}

void *Advance(void *pAddr, ptrdiff_t nOffset, bool bDereference)
{
	if (!pAddr)
		return nullptr;

	void *result = reinterpret_cast<void *>(reinterpret_cast<intptr_t>(pAddr) + nOffset);

	if (bDereference)
		result = *reinterpret_cast<void **>(result);

	return result;
}

void *Resolve(void *pAddr, ptrdiff_t nPreOffset, ptrdiff_t nPostOffset)
{
	if (!pAddr)
		return nullptr;

	return Resolve(reinterpret_cast<void *>(ptrdiff_t(pAddr) + nPreOffset), nPostOffset);
}

void *Resolve(void *pAddr, ptrdiff_t nOffset)
{
	if (!pAddr)
		return nullptr;

	return Advance(pAddr, *static_cast<int *>(pAddr) + nOffset);
}

bool Splice32(void *splice_addr, void *jump_to, void *out_tramp, size_t patch_size, bool place_call)
{
	if (!splice_addr || !jump_to || patch_size < 5) return false;

	uint8_t *src = static_cast<uint8_t *>(splice_addr);
	uint8_t *dst = static_cast<uint8_t *>(jump_to);

	uint8_t *tramp = static_cast<uint8_t *>(
		VirtualAlloc(nullptr, patch_size + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)
		);
	if (!tramp) return false;

	std::memcpy(tramp, src, patch_size);

	uint8_t *tramp_jmp = tramp + patch_size;
	tramp_jmp[0] = 0xE9;
	int32_t rel_back = static_cast<int32_t>((src + patch_size) - (tramp_jmp + 5));
	std::memcpy(tramp_jmp + 1, &rel_back, sizeof(rel_back));

	DWORD old_protect = 0;
	if (!VirtualProtect(src, patch_size, PAGE_EXECUTE_READWRITE, &old_protect))
	{
		VirtualFree(tramp, 0, MEM_RELEASE);
		return false;
	}

	std::memset(src, 0x90, patch_size);

	src[0] = place_call ? 0xE8 : 0xE9;
	int32_t rel_to = static_cast<int32_t>(dst - (src + 5));
	std::memcpy(src + 1, &rel_to, sizeof(rel_to));

	FlushInstructionCache(GetCurrentProcess(), src, patch_size);

	DWORD tmp = 0;
	VirtualProtect(src, patch_size, old_protect, &tmp);

	if (out_tramp)
		*reinterpret_cast<void **>(out_tramp) = static_cast<void *>(tramp);

	return true;
}

MEM_NAMESPACE_END