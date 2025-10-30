#pragma once

#include <stdint.h>

namespace U::Memory
{
	extern void *PtrOffset(const void *addr, ptrdiff_t offset, bool dereference = false);

	extern uint8_t  *FindU8(const void *addr_start, const void *addr_min, const void *addr_max, uint8_t value, bool backward = false, ptrdiff_t offset = 0);
	extern uint16_t *FindU16(const void *addr_start, const void *addr_min, const void *addr_max, uint16_t value, bool backward = false, ptrdiff_t offset = 0);
	extern uint32_t *FindU24(const void *addr_start, const void *addr_min, const void *addr_max, uint8_t value[3], bool backward = false, ptrdiff_t offset = 0);
	extern uint32_t *FindU24(const void *addr_start, const void *addr_min, const void *addr_max, uint32_t value, bool backward = false, ptrdiff_t offset = 0);
	extern uint32_t *FindU32(const void *addr_start, const void *addr_min, const void *addr_max, uint32_t value, bool backward = false, ptrdiff_t offset = 0);

	extern bool WriteU8(void *addr, uint8_t value, ptrdiff_t offset = 0);
	extern bool WriteU16(void *addr, uint16_t value, ptrdiff_t offset = 0);
	extern bool WriteU24(void *addr, uint32_t value, ptrdiff_t offset = 0);
	extern bool WriteU24(void *addr, const int8_t value[3], ptrdiff_t offset = 0);
	extern bool WriteU32(void *addr, uint32_t value, ptrdiff_t offset = 0);
	extern bool WritePointer(void *addr, const void *value, ptrdiff_t offset = 0);

	extern bool CheckU8(const void *addr, uint8_t value, ptrdiff_t offset = 0);
	extern bool CheckU16(const void *addr, uint16_t value, ptrdiff_t offset = 0);
	extern bool CheckU24(const void *addr, uint32_t value, ptrdiff_t offset = 0);
	extern bool CheckU24(const void *addr, const uint8_t value[3], ptrdiff_t offset = 0);
	extern bool CheckU32(const void *addr, uint32_t value, ptrdiff_t offset = 0);

	extern void *Advance(void *pAddr, ptrdiff_t nOffset, bool bDereference = false);

	extern void *Resolve(void *pAddr, ptrdiff_t nPreOffset, ptrdiff_t nPostOffset);
	extern void *Resolve(void *pAddr, ptrdiff_t nOffset = 0);

	extern bool Splice32(void *splice_addr, void *jump_to, void *out_tramp, size_t patch_size, bool place_call = false);
}