#include "pch.h"
#include "Fix.h"

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
    wchar_t szDLLPath[MAX_PATH];

    if (::GetSystemDirectoryW(szDLLPath, ARRAYSIZE(szDLLPath)))
    {
        wcscat_s(szDLLPath, L"\\version.dll");

        if (HMODULE hModVersion = ::LoadLibraryW(szDLLPath))
        {
#define WRAPPER_FUNC(name) orig_##name = ::GetProcAddress(hModVersion, ###name);
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
    }

    ::MessageBoxW(NULL, L"Failed to get your origin dll.\nPlease report this issue on GitHub,\nand remove version.dll from your SA-MP folder until the problem is solved.", L"Whoopsie", MB_OK);
    return false;
}

BOOL WINAPI DllMain(HINSTANCE, DWORD dwReason, LPVOID)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        ::OutputDebugStringW(L"[SA-MP Masterlist Fix] Injected");

        if (InitializeWrapper())
        {
            Fix::GetInstance().EarlyInitialize();
        }
        else
        {
            ::TerminateProcess(::GetCurrentProcess(), 0);
        }
    }
    return TRUE;
}
