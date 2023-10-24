#include "processes.h"

#include <tlhelp32.h>
#include <string>

//const std::wstring leagueProcessNames[] = { L"RiotClientServices.exe", L"LeagueClient.exe" };

std::optional<DWORD> FindLeagueProcess()
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (wcscmp(entry.szExeFile, L"LeagueClient.exe") == 0)
			{
				return entry.th32ProcessID;
			}
		}
	}

	CloseHandle(snapshot);
	return {};
}

void KillLeagueProcess(DWORD processId)
{
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
	if (hProcess != NULL)
	{
		TerminateProcess(hProcess, 1);
		CloseHandle(hProcess);
	}
}
