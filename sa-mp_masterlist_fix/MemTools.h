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

    template <typename T>
    T Rva2Ptr(void* base, size_t offset)
    {
        return reinterpret_cast<T>(static_cast<BYTE*>(base) + offset);
    };

    inline BOOL IATHook(HMODULE dll, char const* targetDLL, void* targetFunction, void* detourFunction)
    {
        IMAGE_DOS_HEADER* mz = (PIMAGE_DOS_HEADER)dll;
        IMAGE_NT_HEADERS* nt = Rva2Ptr<PIMAGE_NT_HEADERS>(mz, mz->e_lfanew);

        IMAGE_IMPORT_DESCRIPTOR* imports = Rva2Ptr<PIMAGE_IMPORT_DESCRIPTOR>(mz, nt->OptionalHeader.DataDirectory[
            IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

        for (int i = 0; imports[i].Characteristics; i++)
        {
            char* name = Rva2Ptr<char*>(mz, imports[i].Name);

            if (lstrcmpiA(name, targetDLL) != 0)
                continue;

            void** thunk = Rva2Ptr<void**>(mz, imports[i].FirstThunk);

            for (; thunk; thunk++)
            {
                void* import = *thunk;

                if (import != targetFunction)
                    continue;

                DWORD oldState;
                if (!::VirtualProtect(thunk, sizeof(void*), PAGE_READWRITE, &oldState))
                    return FALSE;

                *thunk = (void*)detourFunction;

                ::VirtualProtect(thunk, sizeof(void*), oldState, &oldState);

                return TRUE;
            }
        }

        return FALSE;
    }
}
