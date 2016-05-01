#include <Windows.h>
#include <tchar.h>
#include <detours.h>

#pragma comment(lib, "detours.lib")

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) 
{
	DWORD dwPID;
	GetWindowThreadProcessId(hwnd, &dwPID);

	if(dwPID == lParam) 
	{
		SetFocus(hwnd);
		return FALSE;
	}
	return TRUE;
}

BOOL FileExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
			!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	if(!FileExists(TEXT("samp.exe")))
	{
		MessageBox(0, TEXT("SA-MP executable (samp.exe) not found!"), TEXT("Error"), MB_ICONERROR);
		return EXIT_FAILURE;
	}	

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOW;

    DetourCreateProcessWithDllEx(TEXT("samp.exe"), 
                                NULL, NULL, NULL, TRUE, 
                                CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED,
                                NULL, NULL, &si, &pi, 
                                "masterlist_fix.dll", NULL);

    
	ResumeThread(pi.hThread);
	EnumWindows(EnumWindowsProc, (LPARAM)(pi.dwProcessId));
	WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(&si);
    CloseHandle(&pi);

    return EXIT_SUCCESS;
}
