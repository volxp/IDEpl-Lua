#include <ProcessManager.hpp>
               



DWORD ProcessHelper::ProcessManager::getPid(const char* windowname) {
	HWND h = FindWindowA(nullptr, windowname);
	if (!h) return 0;

	DWORD pid;
	GetWindowThreadProcessId(h, &pid);
	if (!pid) return 0;

	return pid;
}




HWND ProcessHelper::ProcessManager::getHwnd(const char* windowname) {
	HWND h = FindWindowA(nullptr, windowname);
	if (!h) return nullptr;
	return h;
}
HANDLE ProcessHelper::ProcessManager::attach(const char* windowname) {
	DWORD pid = getPid(windowname);
	if (!pid) return nullptr;

	HANDLE hp = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hp) return nullptr;

	return hp;
}
HANDLE ProcessHelper::ProcessManager::attach(DWORD pid) {
	HANDLE hp = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hp) return nullptr;

	return hp;
}
HANDLE ProcessHelper::ProcessManager::attach(HWND hwnd) {
	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);
	if (!pid) return nullptr;

	HANDLE hp = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hp) return nullptr;

	return hp;
}

