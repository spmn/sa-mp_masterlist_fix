#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#include <Shlobj.h>
#include <iostream>

void InitializeFix();
void GetRidOfMe();

#define WRAPPER_GENFUNC(name) \
	FARPROC orig_##name; \
	__declspec(naked) void _##name() \
	{ \
		__asm jmp[orig_##name] \
	}

WRAPPER_GENFUNC(GetFileVersionInfoA)
WRAPPER_GENFUNC(GetFileVersionInfoByHandle)
WRAPPER_GENFUNC(GetFileVersionInfoExW)
WRAPPER_GENFUNC(GetFileVersionInfoSizeA)
WRAPPER_GENFUNC(GetFileVersionInfoSizeExW)
WRAPPER_GENFUNC(GetFileVersionInfoSizeW)
WRAPPER_GENFUNC(GetFileVersionInfoW)
WRAPPER_GENFUNC(VerFindFileA)
WRAPPER_GENFUNC(VerFindFileW)
WRAPPER_GENFUNC(VerInstallFileA)
WRAPPER_GENFUNC(VerInstallFileW)
WRAPPER_GENFUNC(VerLanguageNameA)
WRAPPER_GENFUNC(VerLanguageNameW)
WRAPPER_GENFUNC(VerQueryValueA)
WRAPPER_GENFUNC(VerQueryValueW)

bool InitializeWrapper()
{
	char szDLLPath[MAX_PATH];

	if (SUCCEEDED(SHGetFolderPath(NULL,
		CSIDL_SYSTEM,
		NULL,
		0,
		szDLLPath)))
	{
		strcat_s(szDLLPath, MAX_PATH, "\\version.dll");

		HMODULE hModVersion = LoadLibrary(szDLLPath);
#define WRAPPER_FUNC(name) orig_##name = GetProcAddress(hModVersion, ###name);
		WRAPPER_FUNC(GetFileVersionInfoA);
		WRAPPER_FUNC(GetFileVersionInfoByHandle);
		WRAPPER_FUNC(GetFileVersionInfoExW);
		WRAPPER_FUNC(GetFileVersionInfoSizeA);
		WRAPPER_FUNC(GetFileVersionInfoSizeExW);
		WRAPPER_FUNC(GetFileVersionInfoSizeW);
		WRAPPER_FUNC(GetFileVersionInfoW);
		WRAPPER_FUNC(VerFindFileA);
		WRAPPER_FUNC(VerFindFileW);
		WRAPPER_FUNC(VerInstallFileA);
		WRAPPER_FUNC(VerInstallFileW);
		WRAPPER_FUNC(VerLanguageNameA);
		WRAPPER_FUNC(VerLanguageNameW);
		WRAPPER_FUNC(VerQueryValueA);
		WRAPPER_FUNC(VerQueryValueW);

		return true;
	}
	else
	{
		MessageBox(NULL, "Failed to get your origin dll.\nPlease report this issue on github,\nand remove version.dll from your GTA:SA Folder until the problem is solved.", "Whoopsie", MB_OK);
	}

	return false;
}

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{	
		if (InitializeWrapper())
		{
			InitializeFix();
		}
		else
		{
			exit(0);
		}
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		GetRidOfMe();
	}
	return true;
}

/*
this is old code -> would be a "cleaner" way but damn these delphi "AnsiStrings"

#include <string>
char buf[128];
DWORD dwBack = 0x48C7BC;

char *str;

__declspec(naked) void Get(void)
{
__asm mov     edx, str //; System::AnsiStrin
__asm jmp	  [dwBack]
}


#define X86_JMP 0xE9

void Patch(DWORD addr, void* data, size_t size)
{
Protect protect(addr, size);
memcpy(reinterpret_cast<void*>(addr), data, size);
}

void RedirectInstr(DWORD addr, BYTE instr, void * func)
{
DWORD rel_addr = reinterpret_cast<DWORD>(func) - (addr + 5);
Patch(addr, &instr, 1);
Patch(addr + 1, &rel_addr, 4);
}

void ThreadEnter()
{
while (true)
{
str = new char[64];
strcat_s(str, 64, "bitq.eu");

DWORD dwBase = reinterpret_cast<DWORD>(GetModuleHandle(NULL));
dwBack = dwBase + (0x4E6D57 - 0x400000);
dwBase += (0x4E6D52 - 0x400000);


//if (*(BYTE*)(dwBase) == 0xFF)
{
RedirectInstr(dwBase, X86_JMP, Get);
break;
}
}
}
*/