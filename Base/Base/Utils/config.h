#pragma once
#include "offset.h"

struct config
{
	// System
	bool Active = true;

	// ESP
	bool ESP = true;

	// GunMod
	bool GunSway = false;

	// SyncBFSetting
	bool UnlockAll = false;
};

extern config cfg;