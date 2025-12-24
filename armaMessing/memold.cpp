/*
#include "mem.h"


MemInterface::MemInterface() {

}

MemInterface::~MemInterface() {
    CloseHandle(m_Process);
}

DWORD MemInterface::GetProcessID(const wchar_t* ExecutableName) {
    HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (!Snapshot)
        return 0;

    PROCESSENTRY32 ProcessEntry;
    ProcessEntry.dwSize = sizeof(ProcessEntry);

    DWORD ProcessId = 0;

    // Loop all programs on the system, and checks if their executable name is equal to the input parameter.
    if (Process32First(Snapshot, &ProcessEntry)) {
        do {
            printf("%s \n", ProcessEntry.szExeFile);
            if (!_wcsicmp(ExecutableName, ProcessEntry.szExeFile)) {
                ProcessId = ProcessEntry.th32ProcessID;
                break;
            }


        } while (Process32Next(Snapshot, &ProcessEntry));
    }

    CloseHandle(Snapshot);
    return ProcessId;
}
                                      //const char*
UINT64 MemInterface::GetModuleBase(const wchar_t* ModuleName) {
    HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, m_ProcessId);

    if (!Snapshot)
        return 0;

    MODULEENTRY32 ModuleEntry;
    ModuleEntry.dwSize = sizeof(ModuleEntry);

    UINT64 ModuleBase = 0;

    // Loop all programs on the system, and checks if their executable name is equal to the input parameter.
    if (Module32First(Snapshot, &ModuleEntry)) {
        do {

            if (!_wcsicmp(ModuleName, ModuleEntry.szModule)) {
                ModuleBase = reinterpret_cast<UINT64>(ModuleEntry.modBaseAddr);
                break;
            }


        } while (Module32Next(Snapshot, &ModuleEntry));
    }

    CloseHandle(Snapshot);
    return ModuleBase;
}
std::string MemInterface::ReadString(UINT64 Address, UINT32 Size) {
   std::string Buffer = "";
    /*
    auto string = Read<enf_String>(Address);
    std::vector<char>buff = { };
    buff.resize(string.size);
    
    ReadProcessMemory(m_Process, (LPCVOID)string.string, buff.data(), string.size, NULL);
    */
    
    /*
    for (int i = 0; i < Size; i++) {

        char c = Read<char>(Address + i);

        if (c == '\x00')
            break;

        Buffer += c;
    }

    return Buffer;
}

bool MemInterface::Init(const wchar_t* ExecutableName) {
    if (m_ProcessId = GetProcessID(ExecutableName), m_ProcessId != 0)
        m_Process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_ProcessId);

    return m_ProcessId;
}
*/