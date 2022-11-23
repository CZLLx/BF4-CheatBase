#include "battlefield.h"

extern HANDLE pHandle;
BYTE Enable = 1;
BYTE False = 0;
float w_GunSway = 0.0f;

bool IsKeyDown(int VK)
{
	return (GetAsyncKeyState(VK) & 0x8000) != 0;
}

bool IsValid(DWORD_PTR ptr)
{
	if (ptr && HIWORD(ptr) != NULL) return true; else return false;
}

void battlefield::GunMod()
{
	DWORD_PTR Weapon = 0;
	ReadProcessMemory(pHandle, (void*)(OFFSET_WEAPON), &Weapon, sizeof(DWORD_PTR), NULL);
	ReadProcessMemory(pHandle, (void*)(Weapon + 0x128), &Weapon, sizeof(DWORD_PTR), NULL);
	ReadProcessMemory(pHandle, (void*)(Weapon + 0x30), &Weapon, sizeof(DWORD_PTR), NULL);

	if (cfg.GunSway && Weapon)
	{
		WriteProcessMemory(pHandle, (void*)(Weapon + 0x430), &w_GunSway, sizeof(float), NULL);
		WriteProcessMemory(pHandle, (void*)(Weapon + 0x434), &w_GunSway, sizeof(float), NULL);
		WriteProcessMemory(pHandle, (void*)(Weapon + 0x438), &w_GunSway, sizeof(float), NULL);
		WriteProcessMemory(pHandle, (void*)(Weapon + 0x43C), &w_GunSway, sizeof(float), NULL);
	}
}

void battlefield::SyncBFSettingMod()
{
	if (cfg.UnlockAll)
	{
		DWORD_PTR SyncBFSetting = 0;
		ReadProcessMemory(pHandle, (void*)(OFFSET_SYNCBFSETTING), &SyncBFSetting, sizeof(DWORD_PTR), NULL);
		WriteProcessMemory(pHandle, (void*)(SyncBFSetting + 0x54), &w_GunSway, sizeof(BYTE), NULL);
	}
}
