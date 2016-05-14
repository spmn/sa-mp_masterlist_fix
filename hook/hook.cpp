#include <Windows.h>
#include <iostream>
#include <detours.h>

#pragma comment(lib, "detours.lib")
#pragma comment(lib, "ws2_32.lib")

#define MASTERLIST_HOST_DEFAULT "monitor.teamshrimp.com"
#define MASTERLIST_PATH_DEFAULT "/list/masterlist.txt"

int (WINAPI *pSend)(SOCKET s, const char* buf, int len, int flags) = send;
int WINAPI HOOK_send(SOCKET s, const char* buf, int len, int flags);
struct hostent* FAR (WINAPI *pgethostbyname)(const char *cp) = gethostbyname;
struct hostent* FAR WINAPI HOOK_gethostbyname(const char *cp);

char g_szHost[128];
char g_szHTTPHeaders[256+200];
int g_iHTTPHeadersLen;

struct hostent* FAR WINAPI HOOK_gethostbyname(const char *cp)
{
	int iCurTab;
	ReadProcessMemory(GetCurrentProcess(), (LPVOID)0x004EE6E8, &iCurTab, 4, 0); // current tab (fav/internet/hosted)

	if(!strcmp(cp, "lists.sa-mp.com") && iCurTab == 1)
		return pgethostbyname(g_szHost);

	return pgethostbyname(cp);
}

int WINAPI HOOK_send(SOCKET s, const char* buf, int len, int flags)
{
	if(strstr(buf, "GET /0.3.7/servers") != NULL)
		return pSend(s, g_szHTTPHeaders, g_iHTTPHeadersLen, flags);
	
	return pSend(s, buf, len, flags);
}

void LoadConfig()
{
	FILE *fileConfig;
	bool bConfigWritten = false;
	char szPath[128];

	if(fopen_s(&fileConfig, "masterlist_fix.cfg", "r") == 0)
	{
		if(fgets(g_szHost, sizeof(g_szHost), fileConfig) != NULL && fgets(szPath, sizeof(szPath), fileConfig) != NULL)
		{
			g_szHost[strcspn(g_szHost, "\r\n")] = 0;
			szPath[strcspn(szPath, "\r\n")] = 0;

			bConfigWritten = true;
		}
		fclose(fileConfig);
	}

	if(!bConfigWritten)
	{
		strcpy_s(g_szHost, sizeof(g_szHost), MASTERLIST_HOST_DEFAULT);
		strcpy_s(szPath, sizeof(szPath), MASTERLIST_PATH_DEFAULT);
	}

	sprintf_s(g_szHTTPHeaders, sizeof(g_szHTTPHeaders), "\
GET %s HTTP/1.1\r\n\
Content-Type: text/html\r\n\
Host: %s\r\n\
Accept: text/html, */*\r\n\
User-Agent: Mozilla/3.0 (compatible; SA:MP v0.3.7)\r\n\r\n", szPath, g_szHost);

	g_iHTTPHeadersLen = strlen(g_szHTTPHeaders);
}

extern "C" __declspec(dllexport) void dummy(void)
{
    return;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    if(DetourIsHelperProcess()) 
	{
        return TRUE;
    }

    if(dwReason == DLL_PROCESS_ATTACH) 
	{
		LoadConfig();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)pSend, HOOK_send);
        DetourTransactionCommit();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)pgethostbyname, HOOK_gethostbyname);
        DetourTransactionCommit();	
	}
    else if(dwReason == DLL_PROCESS_DETACH)
	{
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)pSend, HOOK_send);
        DetourTransactionCommit();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)pgethostbyname, HOOK_gethostbyname);
        DetourTransactionCommit();
    }
    return TRUE;
}
