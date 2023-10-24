#pragma once

#include <windows.h>
#include <optional>

// Returns process ID of League process if found.
std::optional<DWORD> FindLeagueProcess();

// Kills league process, requires the PID (received from FindLeagueProcess).
void KillLeagueProcess(DWORD processId);