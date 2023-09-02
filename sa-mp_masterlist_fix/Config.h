#pragma once

struct MasterlistEndpoint
{
    MasterlistEndpoint() : m_port(0) { }
    explicit MasterlistEndpoint(const std::string& url);

    std::string GetHostAndPort() const;
    std::string GetUrl() const;
    bool IsValid() const;

    std::string m_proto;
    std::string m_hostname;
    int m_port;
    std::string m_path;

    static constexpr int kDefaultHTTPPort = 80;
};
void from_json(const json& j, MasterlistEndpoint& endpoint);

class Configuration
{
public:
    Configuration();

    bool IsValid() const { return m_isValid; }
    const MasterlistEndpoint& GetInternetEndpoint() const { return m_internetEndpoint; }
    const MasterlistEndpoint& GetHostedEndpoint() const { return m_hostedEndpoint; }

private:
    bool m_isValid;
    MasterlistEndpoint m_internetEndpoint;
    MasterlistEndpoint m_hostedEndpoint;

    static std::filesystem::path GetConfigFilePath();
    static std::string_view GetDefaultConfigContent();

    void Load();

    static constexpr wchar_t kConfigFile[] = L"sa-mp_masterlist_fix.json";
};