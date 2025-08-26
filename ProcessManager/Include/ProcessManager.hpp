#pragma once
#include <Windows.h>
#include <iostream>
       
namespace ProcessHelper {
	class ProcessManager {
	private:

	public:
		static DWORD getPid(const char* windowname);
		static HWND getHwnd(const char* windowname);

		static HANDLE attach(const char* windowname);
		static HANDLE attach(DWORD pid);
		static HANDLE attach(HWND hwnd);
	};
}