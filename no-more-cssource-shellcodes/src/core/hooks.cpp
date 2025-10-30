#include "hooks.hpp"

#include <string>

#include "common/logger.hpp"
#include "core/globals.hpp"

// Must use the '__fastcall' calling convention for 'asm' insertion.
static void __fastcall CheckVoiceDataSize(int size)
{
	if (size > 4096)
		LOG_INFO("Invalid voice data ({}/4096).", size);
}

//
// Prevents the client from downloading files with prohibited extensions (executable files).
//
bool __cdecl H::hkIsSafeFileToDownload(const char *file_name)
{
	auto ext = strchr(file_name, '.');
	if (ext && *ext)
	{
		if (!_strcmpi(ext, ".asi") || !_strcmpi(ext, ".mix") || !_strcmpi(ext, ".flt") || !_strcmpi(ext, ".dll"))
		{
			LOG_INFO("Incoming invalid file '{}'.", file_name);
			return false;
		}
	}

	return G::orgIsSafeFileToDownload(file_name);
}

//
// Stub callback for IE browsers that blocks any pop-up windows
// requesting file downloads through the MOTD window.
//
unsigned int __stdcall H::IEDownload_ThreadProc(void **pParams)
{
	return 0;
}

//
// Blocks shellcode that performs RCE via a buffer overflow in an audio packet.
// Servers using such an attack would download a custom .dll to clients which,
// if I'm not mistaken, replaced the .vdf file and injected the attacker's
// master-server IP address. Haha, classic...
//
__declspec(naked) void H::hkProcessVoiceDataAsm()
{
	__asm
	{
		; Get the packet size in bits, convert bits to bytes,
		; and pass it to the boundary check function
		mov ecx, [ebx + 20]
		lea ecx, [ecx + 7]
		sar ecx, 3
		call CheckVoiceDataSize

		; Save return address in EDX
		pop edx

		; If the packet size in bits is greater or equal to 32767,
		; skip reading data by setting its size to 0
		mov eax, [ebx + 20]
		cmp eax, 8000h
		jl skip_zeroing
		xor eax, eax

		skip_zeroing:
		; Execute part of the original code
		push eax
		lea ecx, [esp + 20]

		; Return to the original function
		jmp edx
	}
}

//
// Blocks loading and execution of all libraries except for two audio codecs.
// This is a common method to load a downloaded DLL onto the client, since
// older versions of voiceinit lacked any filtering.
//
bool __cdecl H::hkVoice_Init(const char *codec_name)
{
	const bool is_empty = (codec_name == nullptr || *codec_name == 0);
	const bool is_miles = !is_empty && (!_strcmpi(codec_name, "vaudio_miles"));
	const bool is_speex = !is_empty && !is_miles && (!_strcmpi(codec_name, "vaudio_speex"));

	if (is_empty)
		return false;

	if (!is_miles && !is_speex)
	{
		LOG_WARNING("Unsafe voice codec library '{}' is requested for initialization.", codec_name);
		return false;
	}

	const bool result = G::orgVoice_Init(codec_name);

	if (result)
		LOG_INFO("Initializing audio codec '{}'.", codec_name);
	else
		LOG_WARNING("Failed to initialize audio codec '{}'.", codec_name);

	return result;
}