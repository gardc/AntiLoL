#include "StartupManager.h"
#include <string>

#define REG_PATH L"Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define APP_NAME L"AntiLeague"

inline std::wstring GetCurrentExecutablePath() {
	wchar_t buffer[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, buffer, MAX_PATH);
	return std::wstring(buffer);
}


bool IsSetToRunOnStartup()
{
	HKEY hkey = NULL;

	if (RegOpenKeyW(HKEY_CURRENT_USER, REG_PATH, &hkey) != ERROR_SUCCESS) {
		return false;
	}

	DWORD dataType;
	if (RegQueryValueExW(hkey, APP_NAME, 0, &dataType, NULL, NULL) == ERROR_SUCCESS) {
		RegCloseKey(hkey);
		return true;
	}

	RegCloseKey(hkey);
	return false;
}

bool AddToStartup()
{
	return false;
}

void EnsureCorrectStartupPath() {
	HKEY hkey = NULL;
	if (RegOpenKeyW(HKEY_CURRENT_USER, REG_PATH, &hkey) != ERROR_SUCCESS) {
		return;
	}

	wchar_t storedPath[MAX_PATH] = { 0 };
	DWORD dataSize = sizeof(storedPath);
	DWORD dataType;

	if (RegQueryValueExW(hkey, APP_NAME, 0, &dataType, (BYTE*)storedPath, &dataSize) == ERROR_SUCCESS) {
		std::wstring currentPath = GetCurrentExecutablePath();

		if (currentPath != storedPath) {
			RegSetValueExW(hkey, APP_NAME, 0, REG_SZ, (BYTE*)currentPath.c_str(), (currentPath.length() + 1) * sizeof(wchar_t));
		}
	}

	RegCloseKey(hkey);
}