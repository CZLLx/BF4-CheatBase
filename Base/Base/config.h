#pragma once
#include "offset.h"

struct config
{
	// System
	bool Active = true;

	// ESP
	bool ESP = true;
	bool TeamESP = false;
	bool VisOnly = false;

	// GunMod
	bool GunSway = false;

};

extern config cfg;