# AntiLoL
A Windows background application (systray) that makes sure League of Legends is never running. If it's found, the LoL client process is terminated.

**Made for fun, obviously, as there is no real utility in this, and to get a refresher in Windows-specific C++.**

## Specs
- C++20
- Windows only, as it is built upon win32 API

## Features
- Checks if League of Legends is running every 5 seconds
	- If it is, terminate the process
	- Shows a notification saying that the process was found (and therefore terminated)
- Run on startup
	- Ensures the startup path is correct when launching application