#pragma once

// Hooks
namespace H
{
	extern bool __cdecl hkIsSafeFileToDownload(const char *file_name);
	extern unsigned int __stdcall IEDownload_ThreadProc(void **pParams);
	extern void hkProcessVoiceDataAsm();
	extern bool __cdecl hkVoice_Init(const char *codec_name);
}