#pragma once

#include <windows.h>

bool IsSetToRunOnStartup();
void EnsureCorrectStartupPath();
void AddToStartup();
void RemoveFromStartup();