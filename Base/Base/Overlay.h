#pragma once
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_win32.h"
#include "ImGui\imgui_impl_dx11.h"
#include <Windows.h>
#include "config.h"

class Overlay
{
public:
	void Menu();
	void ESP();
	void OverlayMain();
	void DrawLine(ImVec2 a, ImVec2 b, ImColor color, float width);
};

