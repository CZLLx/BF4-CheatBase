#include <iostream>
#include "Overlay.h"
#include "battlefield.h"

/* 
	Battlefield 4 external cheat BASE.
	(some of the Japanese lang is because i am Japanese..)

	[ Credit ] 

	 - ImGui
	https://github.com/ocornut/imgui
	 - BF4 offset, pointer and more
	https://www.unknowncheats.me/forum/battlefield-4-a/136527-battlefield-4-ce-table.html
	 - some function and some idea
	https://www.unknowncheats.me/forum/apex-legends/406426-kvm-vmread-apex-esp-aimbot.html
	 - some function
	https://github.com/taxi2za/BF4-Hack
*/

HWND GameHwnd = NULL;
DWORD PID = 0;
HANDLE pHandle = NULL;

Overlay* m = m;
battlefield* BF = BF;

void CheatFunctions()
{
	while (cfg.Active)
	{
		BF->GunMod();				// Example : No Recoil, No Spread
		BF->SyncBFSettingMod();		// Example : UnlockAll

		if (!cfg.Active)
			ExitThread(0);

		Sleep(100); // if you need
	}
}

int main()
{
	// BF4のウィンドウを見つける
	GameHwnd = FindWindowA(NULL, "Battlefield 4");
	if (!GameHwnd)
	{
		MessageBox(NULL, TEXT("Battlefield 4 is not found!"), TEXT(""), MB_ICONWARNING | MB_OK);
		return 0;
	}

	// Get PID
	GetWindowThreadProcessId(GameHwnd, &PID);

	// ProcessHandleを取得
	pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (!pHandle)
	{
		MessageBox(NULL, TEXT("Can't get pHandle!"), TEXT(""), MB_ICONWARNING | MB_OK);
		return 0;
	}

	std::cout << "Ready!\n";
	std::cout << "\nPID : " << PID;

	// Run CheatFuncs
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheatFunctions, NULL, 0, NULL);
	// Run Overlay
	m->OverlayMain();

	return 0;
}