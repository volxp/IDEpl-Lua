#pragma once
#include <Windows.h>
#include <string>
#include <iostream>

struct Process;

class Memory {
public:

	static uintptr_t getModuleBaseAddress(const std::string& modulename);
	static Process getprocess(const char* windowname);
	static std::string rbx_string(uintptr_t addr);
	template <typename T>
	static T read_mem(uintptr_t address, HANDLE hproc) {
		T buf{};
		ReadProcessMemory(hproc, (LPCVOID)address, &buf, sizeof(T), nullptr);
		return buf;
	}
	template <typename T>
	static void write_mem(uintptr_t address, T val, HANDLE hproc) {
		WriteProcessMemory(hproc, (LPVOID)address, &val, sizeof(T), nullptr);
	}

};