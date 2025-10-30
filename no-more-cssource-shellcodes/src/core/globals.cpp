#include "globals.hpp"

#include <Windows.h>
#include <memory>

#include "utils/mem_utils.hpp"
#include "utils/mdl_utils.hpp"

#include "hooks.hpp"

#define SET_PTR(var, mod, offset) \
	(var = reinterpret_cast<decltype(var)>( \
		reinterpret_cast<uintptr_t>(mod.Handle) + static_cast<uintptr_t>(offset)))

static module_t Engine;
static module_t IEFrame;

static void Find()
{
	std::construct_at(&Engine, "engine.dll");
	std::construct_at(&IEFrame, "ieframe.dll");

	//
	// Since the v34 libraries are always binary-stable, I prefer to use
	// static offsets for engine.dll here.
	//
	// The same cannot be said about ieframe.dll - this library may differ
	// from system to system, so I'll perform a simple pattern search just
	// in case.
	//

	SET_PTR(G::pfnIsSafeFileToDownload, Engine, 0xFFF90);
	SET_PTR(G::pfnVoice_Init, Engine, 0x1E4290);
	SET_PTR(G::process_voice_data_asm_placement, Engine, 0x13BFB5);

	if (auto sign = IEFrame.Sig(); true)
	{
		sign.FindU32(0x562C6A53, false, 5)
			.FindU16(0xC084, true, -4);

		sign.CheckU8(0xE8, -1) ? sign.Resolve() : sign.Invalidate();

		G::motd_patch_1 = sign.Ptr;
	}

	if (auto sign = IEFrame.Sig(); true)
	{
		sign.FindU32(0x562C6A53, false, 5);

		if (!sign.IsValid())
			sign.FindU32(0x684075FF, false, 4);

		if (!sign.IsValid())
			sign.Invalidate();

		G::motd_patch_2 = sign.Ptr;
	}
}

static void Hook()
{
	U::Memory::Splice32(G::pfnIsSafeFileToDownload, H::hkIsSafeFileToDownload, &G::orgIsSafeFileToDownload, 5);
	U::Memory::Splice32(G::pfnVoice_Init, H::hkVoice_Init, &G::orgVoice_Init, 6);
	U::Memory::Splice32(G::process_voice_data_asm_placement, H::hkProcessVoiceDataAsm, nullptr, 5, true);

	U::Memory::WriteU32(G::motd_patch_1, 0x90C3C033); // xor eax, eax & ret
	U::Memory::WritePointer(G::motd_patch_2, H::IEDownload_ThreadProc);
}

void InitGlobals()
{
	Find();
	Hook();
}