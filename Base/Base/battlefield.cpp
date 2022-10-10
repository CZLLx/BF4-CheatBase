#include "battlefield.h"

extern HANDLE pHandle;
float w_GunSway = 0.0f;

void battlefield::GunMod()
{
	DWORD_PTR Weapon = 0;
	ReadProcessMemory(pHandle, (void*)(OFFSET_WEAPON), &Weapon, sizeof(DWORD_PTR), NULL);
	ReadProcessMemory(pHandle, (void*)(Weapon + 0x128), &Weapon, sizeof(DWORD_PTR), NULL);
	ReadProcessMemory(pHandle, (void*)(Weapon + 0x30), &Weapon, sizeof(DWORD_PTR), NULL);

	if (Weapon)
	{
		if (cfg.GunSway)
		{
			WriteProcessMemory(pHandle, (void*)(Weapon + 0x430), &w_GunSway, sizeof(float), NULL);
			WriteProcessMemory(pHandle, (void*)(Weapon + 0x434), &w_GunSway, sizeof(float), NULL);
			WriteProcessMemory(pHandle, (void*)(Weapon + 0x438), &w_GunSway, sizeof(float), NULL);
			WriteProcessMemory(pHandle, (void*)(Weapon + 0x43C), &w_GunSway, sizeof(float), NULL);
		}
	}
}

void battlefield::SyncBFSettingMod()
{
	
}
