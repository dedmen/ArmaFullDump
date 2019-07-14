// dllmain.cpp : Defines the entry point for the DLL application.

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#include <vector>
#include <functional>
#include <string_view>
#include <string>

#include <Psapi.h>
#pragma comment (lib, "Psapi.lib")//GetModuleInformation

class HookManager {//Implementation in dllmain
public:

    struct Pattern {
        Pattern(const char* _mask, const char* _pattern) : mask(_mask), pattern(_pattern) {}
        Pattern(const char* _mask, const char* _pattern, int64_t _offset) : mask(_mask), pattern(_pattern), offset(static_cast<uintptr_t>(_offset)) {}
        Pattern(const char* _mask, const char* _pattern, std::function<uintptr_t(uintptr_t)> _offset) : mask(_mask), pattern(_pattern), offsetFunc(_offset) {}
        const char* mask;
        const char* pattern;
        uintptr_t offset{ 0 };
        std::function<uintptr_t(uintptr_t)> offsetFunc;
    };
    HookManager();

    uintptr_t findPattern(const char* pattern, const char* mask, uintptr_t offset = 0) const;
    uintptr_t findPattern(const Pattern& pat, uintptr_t offset = 0) const;

    uintptr_t engineBase;
    uintptr_t engineSize;
};

HookManager::HookManager() {
    MODULEINFO modInfo = { 0 };
    HMODULE hModule = GetModuleHandle(NULL);
    GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));
    engineBase = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
    engineSize = static_cast<uintptr_t>(modInfo.SizeOfImage);
}

uintptr_t HookManager::findPattern(const char* pattern, const char* mask, uintptr_t offset /*= 0*/) const {
    uintptr_t base = engineBase;
    uint32_t size = engineSize;

    uintptr_t patternLength = (DWORD)strlen(mask);

    for (uintptr_t i = 0; i < size - patternLength; i++) {
        bool found = true;
        for (uintptr_t j = 0; j < patternLength; j++) {
            found &= mask[j] == '?' || pattern[j] == *reinterpret_cast<char*>(base + i + j);
            if (!found)
                break;
        }
        if (found)
            return base + i + offset;
    }
    return 0x0;
}

uintptr_t HookManager::findPattern(const Pattern& pat, uintptr_t offset) const {
    if (pat.offsetFunc) {
        auto found = findPattern(pat.pattern, pat.mask, pat.offset + offset);
        if (found)
            return pat.offsetFunc(found);
        return found;
    }

    return findPattern(pat.pattern, pat.mask, pat.offset + offset);
}

HookManager::Pattern pat_createMdmp{
    "xxx????????xxxx????xxxx????xx????xxx????xxxx????xxxxxxx????x????xxxx????xxxx????x????xxxx?xxxx????xxxx?xxxx?xxxx????xxxx?xxxxxxxxxxx?xxxx?xxxxxx?xxxxxxx?xxxx?xxxxxxx?xxx??xxxxxx????x????xxxx????x????xxx????xxxxxxx????xxxxxx????xxxx????x????xxxx????xxxx????x????xxxx????xxxx?????xxx?????xxx?????xxxxx????x????xxxx????xx????xxxx?xxxx????x????xxxx??xx????xxxxxxxx????xxxxxx????xxxx?????xxxx?????xxxx????xxxx?xx????xxxx?xxxxxxxxxxxxxxxxxxxxx????x????xxxxxx",
    "\xC7\x84\x24\x00\x00\x00\x00\x00\x00\x00\x00\x48\x8B\x84\x24\x00\x00\x00\x00\x48\x89\x84\x24\x00\x00\x00\x00\xFF\x15\x00\x00\x00\x00\x89\x84\x24\x00\x00\x00\x00\x48\x8B\x84\x24\x00\x00\x00\x00\x48\x8B\xD0\x48\x8D\x8C\x24\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x89\x84\x24\x00\x00\x00\x00\x48\x8B\x8C\x24\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x89\x44\x24\x00\x48\x8D\x84\x24\x00\x00\x00\x00\x48\x89\x44\x24\x00\x48\x8B\x44\x24\x00\x48\x89\x84\x24\x00\x00\x00\x00\x48\x8B\x44\x24\x00\x0F\xB6\x00\x88\x44\x24\x40\x48\x8B\x44\x24\x00\x0F\xB6\x4C\x24\x00\x88\x08\x48\x8B\x44\x24\x00\x48\xFF\xC0\x48\x89\x44\x24\x00\x48\x8B\x44\x24\x00\x48\xFF\xC0\x48\x89\x44\x24\x00\x80\x7C\x24\x00\x00\x75\xC7\x48\x8D\x8C\x24\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8D\x8C\x24\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\x48\x8B\xF8\x48\x8B\xF1\xB9\x00\x00\x00\x00\xF3\xA4\x48\x8D\x94\x24\x00\x00\x00\x00\x48\x8D\x8C\x24\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x89\x84\x24\x00\x00\x00\x00\x48\x8B\x8C\x24\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x89\x84\x24\x00\x00\x00\x00\x48\xC7\x44\x24\x00\x00\x00\x00\x00\xC7\x44\x24\x00\x00\x00\x00\x00\xC7\x44\x24\x00\x00\x00\x00\x00\x45\x33\xC9\x41\xB8\x00\x00\x00\x00\xBA\x00\x00\x00\x00\x48\x8B\x8C\x24\x00\x00\x00\x00\xFF\x15\x00\x00\x00\x00\x48\x89\x44\x24\x00\x48\x8D\x8C\x24\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x83\x7C\x24\x00\x00\x0F\x84\x00\x00\x00\x00\x33\xC0\x85\xC0\x75\xFA\xFF\x15\x00\x00\x00\x00\x89\x44\x24\x60\xFF\x15\x00\x00\x00\x00\x48\xC7\x44\x24\x00\x00\x00\x00\x00\x48\xC7\x44\x24\x00\x00\x00\x00\x00\x48\x8D\x8C\x24\x00\x00\x00\x00\x48\x89\x4C\x24\x00\x41\xB9\x00\x00\x00\x00\x4C\x8B\x44\x24\x00\x8B\x4C\x24\x60\x8B\xD1\x48\x8B\xC8\xFF\x54\x24\x78\x85\xC0\x74\x2D\x48\x8D\x8C\x24\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x4C\x8B\xC0\x48\x8D\x15",
    [](uintptr_t found) -> uintptr_t {
        //found DB06
        //target mov DC9B
        //target B DC9D

        //uint32_t* data = reinterpret_cast<uint32_t*>(found + 0x197);
        return found + 0x197;
    }
};

extern "C"
{
    __declspec(dllexport) void __stdcall RVExtension(char* output, int outputSize, const char* function);
    __declspec(dllexport) void __stdcall RVExtensionVersion(char* output, int outputSize);
}

#include<DbgHelp.h>

void __stdcall RVExtension(char* output, int outputSize, const char* function) {
    if (std::string_view(function) == "crash") {
        int* x = reinterpret_cast<int*>(5);
        volatile int a = 0;
        *x = 1 / a;
    }

    std::string ret = "fail";

    HookManager mang;
    uint32_t* dumpType = reinterpret_cast<uint32_t*>(mang.findPattern(pat_createMdmp));

    if (dumpType != nullptr) {
        ret = "success";
        DWORD dwVirtualProtectBackup;
        VirtualProtect(reinterpret_cast<LPVOID>(dumpType), 4u, 0x40u, &dwVirtualProtectBackup);
        *dumpType = MiniDumpNormal | MiniDumpWithDataSegs | MiniDumpWithFullMemory;// | MiniDumpIgnoreInaccessibleMemory | MiniDumpWithModuleHeaders;
        VirtualProtect(reinterpret_cast<LPVOID>(dumpType), 4u, dwVirtualProtectBackup, &dwVirtualProtectBackup);
    }

    std::strncpy(output, ret.c_str(), ret.length());
}
void __stdcall RVExtensionVersion(char* output, int outputSize) {
    std::strncpy(output, "Arma full mdmp", outputSize - 1);
}
