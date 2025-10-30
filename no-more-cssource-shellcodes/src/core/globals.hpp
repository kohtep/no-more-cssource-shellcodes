#pragma once

// Globals
namespace G
{
	namespace Internal
	{
		using IsSafeFileToDownload_t = bool(__cdecl *)(const char *file_name);
		using Voice_Init_t = bool(__cdecl *)(const char *codec_name);
	}

	inline Internal::IsSafeFileToDownload_t pfnIsSafeFileToDownload;
	inline Internal::IsSafeFileToDownload_t orgIsSafeFileToDownload;

	inline Internal::Voice_Init_t pfnVoice_Init;
	inline Internal::Voice_Init_t orgVoice_Init;

	inline void *process_voice_data_asm_placement;

	inline void *motd_patch_1; // CDownloadUtilities::OpenUI
	inline void *motd_patch_2; // IEDownload::ThreadProc
}

extern void InitGlobals();