#include "pch.h"
#include "Fix.h"
#include "MemTools.h"

FARPROC WINAPI Fix::GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
    auto& self = GetInstance();
    urmem::hook::raii raii(self.m_procGetterHook);

    auto it = self.m_hooks.find(lpProcName);
    if (it != self.m_hooks.end())
    {
        return it->second;
    }

    return ::GetProcAddress(hModule, lpProcName);
}

struct hostent* WINAPI Fix::gethostbyname(const char* cp)
{
    auto& self = GetInstance();

    if (!strncmp(cp, kDefaultMasterlist, std::string::traits_type::length(kDefaultMasterlist)))
    {
        const auto& newHostname = self.GetEndpointForCurrentTab().m_hostname;
        if (!newHostname.empty())
        {
            auto host = ::gethostbyname(newHostname.c_str());
            if (host && host->h_addr_list && host->h_addr_list[0])
            {
                self.m_resolvedAddress = *reinterpret_cast<in_addr*>(host->h_addr_list[0]);
                return host;
            }
        }
    }

    return ::gethostbyname(cp);
}

int WINAPI Fix::connect(SOCKET s, const sockaddr* name, int namelen)
{
    auto& self = GetInstance();
    auto nameIpv4 = reinterpret_cast<sockaddr_in*>(const_cast<sockaddr*>(name));

    if (nameIpv4 && namelen == sizeof(*nameIpv4) && self.m_resolvedAddress.S_un.S_addr == nameIpv4->sin_addr.S_un.S_addr)
    {
        if (int newPort = self.GetEndpointForCurrentTab().m_port)
        {
            nameIpv4->sin_port = ::htons(static_cast<short>(newPort));

            assert(self.m_socket == INVALID_SOCKET && "New socket opened without closing the previous one");
            self.m_socket = s;
        }
    }
    return ::connect(s, name, namelen);
}

int WINAPI Fix::send(SOCKET s, const char* buf, int len, int flags)
{
    using namespace std::string_literals;
    auto& self = GetInstance();

    if (self.m_socket == s)
    {
        auto view = std::string_view{ buf, static_cast<size_t>(len) };
        std::string prefix = "GET /"s + self.m_browser.GetVersion() + "/";

        if (view.starts_with(prefix))
        {
            auto request = self.GetHTTPRequestForCurrentTab();
            if (!request.empty())
            {
                return ::send(s, request.c_str(), request.size(), flags);
            }
        }
    }

    return ::send(s, buf, len, flags);
}

int WINAPI Fix::closesocket(SOCKET s)
{
    auto& self = GetInstance();

    if (self.m_socket == s)
    {
        self.m_socket = INVALID_SOCKET;
    }

    return ::closesocket(s);
}

MasterlistEndpoint Fix::GetEndpointForCurrentTab() const
{
    switch (m_browser.GetTab())
    {
    case ServerBrowser::Tab::Internet:
        return m_config.GetInternetEndpoint();
    case ServerBrowser::Tab::Hosted:
        return m_config.GetHostedEndpoint();
    }

    return MasterlistEndpoint{};
}

std::string Fix::GetHTTPRequestForCurrentTab() const
{
    auto endpoint = GetEndpointForCurrentTab();
    if (!endpoint.IsValid())
    {
        return "";
    }

    static const std::regex regexp{ "\\$\\(version\\)" };
    auto path = std::regex_replace(endpoint.m_path, regexp, m_browser.GetVersion());

    return std::format(
"GET {} HTTP/1.1\r\n\
Content-Type: text/html\r\n\
Host: {}\r\n\
Accept: text/html, */*\r\n\
User-Agent: Mozilla/3.0 (compatible; SA:MP v{})\r\n\r\n",
        path,
        endpoint.GetHostAndPort(),
        m_browser.GetVersion()
    );
}

#define DECLARE_NET_HOOK(hook) { #hook, reinterpret_cast<FARPROC>(&Fix::hook) }
Fix::Fix()
    : m_hooks
    ({
        DECLARE_NET_HOOK(gethostbyname),
        DECLARE_NET_HOOK(connect),
        DECLARE_NET_HOOK(send),
        DECLARE_NET_HOOK(closesocket)
    })
    , m_resolvedAddress{}
    , m_socket{ INVALID_SOCKET }
{
}

Fix& Fix::GetInstance()
{
    static Fix instance;
    return instance;
}

bool Fix::Initialize()
{
    if (!m_browser.IsSupportedProcess())
    {
        // We're injected into a random process or in an unsupported SA-MP server browser version
        // Either way, we should not try to mess with the current process any longer
        return false;
    }

    if (!m_config.IsValid())
    {
        assert(false && "Invalid configuration");
        return false;
    }

    urmem::address_t realGetProcAddress = urmem::get_func_addr(::GetProcAddress);
    urmem::address_t hookGetProcAddress = urmem::get_func_addr(Fix::GetProcAddress);

    m_procGetterHook.install(realGetProcAddress, hookGetProcAddress);
    return true;
}
