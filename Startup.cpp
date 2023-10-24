#include "Startup.h"
#include <string>
#include <iostream>

constexpr auto REG_PATH = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
constexpr auto APP_NAME = L"AntiLoL";

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

void AddToStartup()
{
	HKEY hkey = NULL;
	wchar_t appPath[MAX_PATH] = { 0 };

	// Get the path of the current executable
	GetModuleFileNameW(NULL, appPath, MAX_PATH);

	// Open the registry key
	if (RegOpenKeyW(HKEY_CURRENT_USER, REG_PATH, &hkey) != ERROR_SUCCESS) {
		std::cout << "Failed to open registry key" << std::endl;
		return;
	}

	// Set the value
	if (RegSetValueExW(hkey, APP_NAME, 0, REG_SZ, (BYTE*)appPath, (wcslen(appPath) + 1) * sizeof(wchar_t)) != ERROR_SUCCESS) {
		RegCloseKey(hkey);
		std::cout << "Failed to write startup register value" << std::endl;
		return;
	}

	RegCloseKey(hkey);
	return;
}

void RemoveFromStartup()
{
	HKEY hkey = NULL;

	// Open the registry key
	if (RegOpenKeyW(HKEY_CURRENT_USER, REG_PATH, &hkey) != ERROR_SUCCESS) {
		std::cout << "Failed to open registry key" << std::endl;
		return;
	}

	// Delete the value
	if (RegDeleteValueW(hkey, APP_NAME) != ERROR_SUCCESS) {
		RegCloseKey(hkey);
		std::cout << "Failed to delete register value" << std::endl;
		return;
	}

	RegCloseKey(hkey);
	return;
}

void EnsureCorrectStartupPath() {
	HKEY hkey = NULL;
	if (RegOpenKeyW(HKEY_CURRENT_USER, REG_PATH, &hkey) != ERROR_SUCCESS) {
		std::cout << "Failed to open registry key" << std::endl;
		return;
	}

	wchar_t storedPath[MAX_PATH] = { 0 };
	DWORD dataSize = sizeof(storedPath);
	DWORD dataType;

	if (RegQueryValueExW(hkey, APP_NAME, 0, &dataType, (BYTE*)storedPath, &dataSize) == ERROR_SUCCESS) {
		std::wstring currentPath = GetCurrentExecutablePath();

		// If the stored path is different from the current path, update the registry value
		if (currentPath != storedPath) {
			RegSetValueExW(hkey, APP_NAME, 0, REG_SZ, (BYTE*)currentPath.c_str(), (currentPath.length() + 1) * sizeof(wchar_t));
		}
	}

	RegCloseKey(hkey);
}