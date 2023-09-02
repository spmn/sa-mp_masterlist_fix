#include "pch.h"
#include "ServerBrowser.h"
#include "MemTools.h"

namespace
{
    template <size_t PatternLength>
    constexpr auto MakeFullMask()
    {
        std::array<char, PatternLength+1> a;

        for (auto& x : a)
            x = 'x';

        a[PatternLength] = 0;
        return a;
    }
}

ServerBrowser::ServerBrowser()
    : m_tab(nullptr)
{
    urmem::sig_scanner scanner;
    if (!scanner.init(urmem::get_func_addr(::GetModuleHandleW(nullptr))))
    {
        return;
    }

    m_version = GetVersion(scanner);
    m_tab = GetTabAddress(scanner);
}

std::string ServerBrowser::GetVersion(const urmem::sig_scanner& scanner)
{
    static constexpr char kPattern[] = "Mozilla/3.0 (compatible; SA:MP v";
    static constexpr auto kPatternLength = std::string::traits_type::length(kPattern);
    static constexpr auto kMask = MakeFullMask<kPatternLength>();

    urmem::address_t patternAddress;
    if (!scanner.find(kPattern, kMask.data(), patternAddress))
    {
        return "";
    }

    const char* version = reinterpret_cast<const char*>(patternAddress + kPatternLength);

    auto versionInfo = MemTools::QueryObjectMemory(version);
    if (!MemTools::IsReadable(versionInfo))
    {
        return "";
    }
    if (versionInfo.AllocationBase != ::GetModuleHandleW(nullptr))
    {
        return "";
    }

    size_t maxSearch = MemTools::GetRemainingBytesWithSameProtection(version, versionInfo);
    auto accessibleRange = std::string_view{ version, maxSearch };
    auto end = accessibleRange.find(')');
    if (end == std::string_view::npos)
    {
        return "";
    }

    return std::string{ accessibleRange.substr(0, end) };
}

ServerBrowser::Tab* ServerBrowser::GetTabAddress(const urmem::sig_scanner& scanner)
{
    // IDA pattern: 83 3D ? ? ? ? 02 75 ? 8D 4D
    static constexpr char kPattern[] = "\x83\x3D\x00\x00\x00\x00\x02\x75\x00\x8D\x4D";
    static constexpr char kMask[] = "xx????xx?xx";
    static constexpr int kTabAddressOffset = 2;

    urmem::address_t tabPatternAddress;
    if (!scanner.find(kPattern, kMask, tabPatternAddress))
    {
        return nullptr;
    }

    Tab** tab = reinterpret_cast<Tab**>(tabPatternAddress + kTabAddressOffset);

    auto codeInformation = MemTools::QueryObjectMemory(tab);
    if (!MemTools::IsExecutable(codeInformation))
    {
        return nullptr;
    }
    if (codeInformation.AllocationBase != ::GetModuleHandleW(nullptr))
    {
        return nullptr;
    }

    auto tabInformation = MemTools::QueryObjectMemory(*tab);
    if (!MemTools::IsWritable(tabInformation))
    {
        return nullptr;
    }
    if (tabInformation.AllocationBase != ::GetModuleHandleW(nullptr))
    {
        return nullptr;
    }

    return *tab;
}
