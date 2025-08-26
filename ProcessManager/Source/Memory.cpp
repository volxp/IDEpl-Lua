#include "Memory.hpp"
#include <string>
#include <iostream>
#include <TlHelp32.h>
#include <RobloxManager.hpp>
#include <Log.hpp>


Process Memory::getprocess(const char* windowname) {
    DWORD pid;
    current.WindowHandle = FindWindowA(nullptr, windowname);
    if (!current.WindowHandle) {
        Log::debug("Failed to find %s, is the process open?", windowname);
        exit(EXIT_FAILURE); // syscall 80
    }

    GetWindowThreadProcessId(current.WindowHandle, &pid);
    if (!pid) {
        Log::debug("Failed to get PID for %s!", windowname);
        exit(EXIT_FAILURE);
    }
    Log::debug("Found %s with PID: %lu", windowname, pid);

    current.ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!current.ProcessHandle) {
        Log::debug("Failed to attach to process! Error: %lu", GetLastError());
        exit(EXIT_FAILURE);
    }
    Log::debug("Opened ProcessHandle: 0x%llx", current.ProcessHandle);

    current.SelfModule = getModuleBaseAddress("RobloxPlayerBeta.exe");
    if (!current.SelfModule) {
        Log::debug("Failed to get module base address!");
        exit(EXIT_FAILURE);
    }
    Log::debug("Found SelfModule @ 0x%llx", current.SelfModule);

    return current;
}

uintptr_t Memory::getModuleBaseAddress(const std::string& modulename) {
    uintptr_t baseAddress = 0;
    DWORD pid = 0;

    GetWindowThreadProcessId(current.WindowHandle, &pid);
    if (!pid) {
        Log::debug("Failed to retrieve PID for module search.");
        return 0;
    }

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        Log::debug("Failed to create module snapshot. Error: %lu", GetLastError());
        return 0;
    }

    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &moduleEntry)) {
        do {
            if (!_stricmp(moduleEntry.szModule, modulename.c_str())) {
                baseAddress = reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
                break;
            }
        } while (Module32Next(hSnapshot, &moduleEntry));
    }
    else {
        Log::debug("Failed to enumerate modules. Error: %lu", GetLastError());
    }

    CloseHandle(hSnapshot);
    return baseAddress;
}


std::string Memory::rbx_string(uintptr_t addr) {
    int32_t check = read_mem<int32_t>(addr + 0x18, current.ProcessHandle);
    if (check > 15)
        addr = read_mem<uintptr_t>(addr, current.ProcessHandle);

    std::string name = "";
    for (int j = 0; j < 640; j++) {
        char c = read_mem<char>(addr + j, current.ProcessHandle);
        if (c == '\0') break;
        name.push_back(c);
    }

    return name;
}