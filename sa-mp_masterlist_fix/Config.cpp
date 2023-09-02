#include "pch.h"
#include "Config.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

Configuration::Configuration()
    : m_isValid(false)
{
    Load();
}

std::filesystem::path Configuration::GetConfigFilePath()
{
    char userDocuments[MAX_PATH];

    // Deprecated and unicode-unfriendly, but that's what SA-MP does
    if (::SHGetSpecialFolderPathA(nullptr, userDocuments, CSIDL_PERSONAL, FALSE))
    {
        return std::filesystem::path{ userDocuments } / "GTA San Andreas User Files" / "SAMP" / kConfigFile;
    }

    return std::filesystem::path{ kConfigFile };
}

std::string_view Configuration::GetDefaultConfigContent()
{
    HMODULE hDll = reinterpret_cast<HMODULE>(&__ImageBase);
    HRSRC hResource = ::FindResourceW(hDll, MAKEINTRESOURCEW(IDR_CONFIG), L"JSON");
    HGLOBAL hLoadedResource = LoadResource(hDll, hResource);
    const char* jsonContent = reinterpret_cast<const char*>(LockResource(hLoadedResource));
    DWORD jsonSize = SizeofResource(hDll, hResource);

    if (jsonContent == nullptr || jsonSize == 0)
    {
        return std::string_view{};
    }

    return std::string_view{ jsonContent, jsonSize };
}

void Configuration::Load()
{
    auto configPath = GetConfigFilePath();
    auto defaultConfigContent = GetDefaultConfigContent();
    auto defaultConfig = json::parse(defaultConfigContent);

    if (std::ifstream ifs{ configPath })
    {
        json config = json::parse(ifs);
        config["endpoints"]["internet"].get_to(m_internetEndpoint);
        config["endpoints"]["hosted"].get_to(m_hostedEndpoint);
    }
    else
    {
        // Missing config file.
        // Let's try to create and populate it with the default values.
        if (std::ofstream ofs{ configPath, std::ios::binary })
        {
            ofs.write(defaultConfigContent.data(), defaultConfigContent.size());
        }
    }

    // Populate any invalid setting with the equivalent from the default config
    if (!m_internetEndpoint.IsValid())
    {
        defaultConfig["endpoints"]["internet"].get_to(m_internetEndpoint);
    }

    if (!m_hostedEndpoint.IsValid())
    {
        defaultConfig["endpoints"]["hosted"].get_to(m_hostedEndpoint);
    }

    m_isValid = m_internetEndpoint.IsValid() && m_hostedEndpoint.IsValid();
}

MasterlistEndpoint::MasterlistEndpoint(const std::string& url)
    : m_port(0)
{
    static const std::regex regexp{ "^(?:([A-Za-z0-9\\+\\-\\.]*)://)?([A-Za-z0-9\\-\\.]+)(?::([0-9]+))?(.*)$" };
    std::smatch matches;

    if (std::regex_match(url, matches, regexp) && matches.size() == 5)
    {
        m_proto = matches[1].str();
        m_hostname = matches[2].str();
        m_port = matches[3].matched ? std::atoi(matches[3].str().c_str()) : kDefaultHTTPPort;
        m_path = matches[4].str();

        std::transform(m_proto.begin(), m_proto.end(), m_proto.begin(), ::tolower);
    }
}

std::string MasterlistEndpoint::GetHostAndPort() const
{
    using namespace std::string_literals;

    std::string port = (m_port == kDefaultHTTPPort)
        ? ""s
        : (":"s + std::to_string(m_port));

    return m_hostname + port;
}

std::string MasterlistEndpoint::GetUrl() const
{
    return m_proto + "://" + GetHostAndPort() + m_path;
}

bool MasterlistEndpoint::IsValid() const
{
    return (m_proto == "http") && !m_hostname.empty() && m_port > 0 && m_port < 65536 && m_path.starts_with("/");
}

void from_json(const json& j, MasterlistEndpoint& endpoint)
{
    try
    {
        std::string url;
        j.at("url").get_to(url);
        endpoint = MasterlistEndpoint(url);
    }
    catch (const json::exception&)
    {
    }
}
