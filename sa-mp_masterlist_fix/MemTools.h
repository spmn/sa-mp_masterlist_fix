#pragma once

namespace MemTools
{
    inline bool IsReadable(const MEMORY_BASIC_INFORMATION& mbi)
    {
        DWORD protect = mbi.Protect;
        return protect == PAGE_READONLY || protect == PAGE_READWRITE || protect == PAGE_WRITECOPY || protect == PAGE_EXECUTE_READ || protect == PAGE_EXECUTE_READWRITE || protect == PAGE_EXECUTE_WRITECOPY;
    }

    inline bool IsWritable(const MEMORY_BASIC_INFORMATION& mbi)
    {
        DWORD protect = mbi.Protect;
        return protect == PAGE_READWRITE || protect == PAGE_EXECUTE_READWRITE || protect == PAGE_WRITECOPY;
    }

    inline bool IsExecutable(const MEMORY_BASIC_INFORMATION& mbi)
    {
        DWORD protect = mbi.Protect;
        return protect == PAGE_EXECUTE || protect == PAGE_EXECUTE_READ || protect == PAGE_EXECUTE_READWRITE || protect == PAGE_EXECUTE_WRITECOPY;
    }

    inline size_t GetRemainingBytesWithSameProtection(const void* address, const MEMORY_BASIC_INFORMATION& mbi)
    {
        size_t usedBytes = static_cast<LPCBYTE>(address) - static_cast<LPCBYTE>(mbi.BaseAddress);
        return mbi.RegionSize - usedBytes;
    }

    template <typename T>
    MEMORY_BASIC_INFORMATION QueryObjectMemory(const T* address)
    {
        MEMORY_BASIC_INFORMATION mbi{};

        if (::VirtualQuery(address, &mbi, sizeof(mbi))
            && GetRemainingBytesWithSameProtection(address, mbi) >= sizeof(T))
        {
            return mbi;
        }

        return {};
    }
}
