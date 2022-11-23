#pragma once
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_win32.h"
#include "ImGui\imgui_impl_dx11.h"
#include <Windows.h>
#include <dwmapi.h>
#include <d3dx9.h>
#include <d3d11.h>
#include "Utils\config.h"
#pragma comment(lib, "d3d11.lib")

#define GREEN ImColor(0, 255, 0)
#define RED ImColor(255, 0, 0)
#define BLUE ImColor(0, 0, 255)
#define SKYBLUE ImColor(0, 255, 255)

static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool WorldToScreen(const D3DXVECTOR3& WorldPos, D3DXVECTOR3& ScreenPos);

class Draw
{
public:
	void DrawLine(ImVec2 a, ImVec2 b, ImColor color, float width)
	{
		ImGui::GetWindowDrawList()->AddLine(a, b, color, width);
	}
};

