#pragma once

class ServerBrowser
{
public:
    enum class Tab : int
    {
        Favorites = 0,
        Internet  = 1,
        Hosted    = 2,
        Unknown
    };

    ServerBrowser();

    const std::string& GetVersion() const { return m_version; }
    Tab GetTab() const { return m_tab ? *m_tab : Tab::Unknown; }
    bool IsSupportedProcess() const { return !m_version.empty() && m_tab; }

private:
    static std::string GetVersion(const urmem::sig_scanner& scanner);
    static Tab* GetTabAddress(const urmem::sig_scanner& scanner);

    std::string m_version;
    Tab* m_tab;
};

