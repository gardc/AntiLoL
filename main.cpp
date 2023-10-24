#include <stdio.h>
#include <time.h>
#include <iostream>
#include <thread>

#include "Processes.h"
#include "StartupManager.h"

// Global variables are generally to be avoided, but this is a quick n dirty program that won't be expanded upon... hopefully.

HINSTANCE hInst; // current instance
NOTIFYICONDATA nid = {}; // notification icon data
std::thread lolProcessCheckerThread; // thread that checks for the league process, to be initialized in WinMain
std::atomic_bool stopThreadSignal = false; // used to signal the thread(s) to stop
bool runOnStartup = false; // used to determine if the program should run on startup

#define ID_RUN_ON_STARTUP 1
#define ID_QUIT 2


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = WM_USER;
		nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcscpy_s(nid.szTip, L"AntiLoL");
		Shell_NotifyIcon(NIM_ADD, &nid);
		break;

	case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
		break;

	case WM_USER:
		if (lParam == WM_RBUTTONUP) {
			POINT pt;
			GetCursorPos(&pt);
			HMENU hMenu = CreatePopupMenu();

			runOnStartup = IsSetToRunOnStartup();  // Check current status
			AppendMenu(hMenu, MF_STRING | (runOnStartup ? MF_CHECKED : MF_UNCHECKED), ID_RUN_ON_STARTUP, L"Run on System Startup");
			AppendMenu(hMenu, MF_STRING, ID_QUIT, L"Quit");

			SetForegroundWindow(hWnd);
			TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, pt.x, pt.y, 0, hWnd, NULL);
			DestroyMenu(hMenu);
		}
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_RUN_ON_STARTUP) {
			if (runOnStartup) {
				RemoveFromStartup();  // This function removes your app from startup. It's similar to AddToStartup but uses RegDeleteValue.
			}
			else {
				AddToStartup();
			}
			runOnStartup = !g_bRunOnStartup;  // Toggle the status
		}
		else if (LOWORD(wParam) == ID_QUIT) {
			// Your quit logic here...
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void LoLProcessChecker() {
	auto leagueProcessId = FindLeagueProcess();
	auto statusMessage = leagueProcessId.has_value() ? "found(" + leagueProcessId.value() : "not found";

	std::cout << "league process id: " << statusMessage << std::endl;

	// create loop that runs every 5 seconds
	while (true) {
		// check if the process should be stopped
		if (stopThreadSignal) {
			break;
		}

		// get league process id
		auto leagueProcessId = FindLeagueProcess();

		// if league process id is found
		if (leagueProcessId.has_value()) {
			// kill league process
			KillLeagueProcess(leagueProcessId.value());
			std::cout << "found the league... destroying..." << std::endl;

			// show notification
			nid.uFlags = NIF_INFO;
			wcscpy_s(nid.szInfoTitle, L"AntiLoL");
			wcscpy_s(nid.szInfo, L"League of Legends was detected... Nope nopety nope >:|");
			nid.dwInfoFlags = NIIF_WARNING; // Use NIIF_ERROR, NIIF_WARNING, or NIIF_INFO for different icons
			//nid.hIcon = LoadIcon(NULL, IDI_ERROR);
			Shell_NotifyIcon(NIM_MODIFY, &nid);
		}

		// sleep for 5 seconds
		Sleep(5000);
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// Create a console windows if debug
#ifdef _DEBUG
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
	std::cout << "Debug Console" << std::endl;
#endif

	hInst = hInstance;
	const wchar_t CLASS_NAME[] = L"AntiLoL";

	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);

	HWND hWnd = CreateWindowEx(0, CLASS_NAME, L"AntiLoL", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);

	// spawn the process checker thread
	lolProcessCheckerThread = std::thread(LoLProcessChecker);

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	lolProcessCheckerThread.join();

	return 0;
}