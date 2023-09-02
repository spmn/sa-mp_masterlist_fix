#pragma once

#include "Config.h"
#include "ServerBrowser.h"

class Fix
{
public:
    static Fix& GetInstance();
    bool Initialize();

private:
    Fix();
    Fix(const Fix&) = delete;
    Fix(Fix&&) = delete;
    void operator=(const Fix&) = delete;
    void operator=(Fix&&) = delete;

    // Hooked functions
    static FARPROC WINAPI GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

    static struct hostent* WINAPI gethostbyname(const char* cp);
    static int WINAPI connect(SOCKET s, const sockaddr* name, int namelen);
    static int WINAPI send(SOCKET s, const char* buf, int len, int flags);
    static int WINAPI closesocket(SOCKET s);

    MasterlistEndpoint GetEndpointForCurrentTab() const;
    std::string GetHTTPRequestForCurrentTab() const;

    urmem::hook m_procGetterHook;
    std::unordered_map<std::string, FARPROC> m_hooks;
    Configuration m_config;
    ServerBrowser m_browser;

    in_addr m_resolvedAddress;
    SOCKET m_socket;

    static constexpr char kDefaultMasterlist[] = "lists.sa-mp.com";
};
