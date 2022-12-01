#include <iostream>
#include "battlefield.h"

/* 
	Battlefield 4 external cheat BASE.

	[ Note ]
	・今回はint main() 使ってますけど、普通にWINAPI WinMainでも大丈夫です。
	・ReadProcessMemoryをESPの関数で呼び出しているので必然的にESPのパフォーマンスが落ちます。(144Hzモニターでプレイしても快適でしたが、、)
	　プレイヤーの情報を取得する関数を作って、そこで取得した情報を構造体に入れてESP関数をレンダリング専用にするという手もあります。(自分はそうしてます)
	　まあどちらにせよESP等のレンダリングする関数内でRPMを呼び出すのはパフォーマンス的に良くはないよねっていう、
	
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

// Class
Draw d;
battlefield BF;

// ESP Color
ImVec4 ESP_Visible = ImVec4(1.f, 0.f, 0.f, 1.f); // R, G, B, A
ImVec4 ESP_Normal = ImVec4(0.f, 1.f, 0.f, 1.f);

HWND hwnd = NULL;      // Overlay hwnd
HWND GameHwnd = NULL;  // BF4 Window
DWORD PID = 0;		   // BF4 ProcessID
HANDLE pHandle = NULL; // BF4 Process Handle

bool ShowMenu = false;

extern bool IsKeyDown(int VK);
extern bool IsValid(DWORD_PTR ptr);

// Cheat thread function
void CheatFunctions()
{
	while (cfg.Active)
	{
		BF.GunMod();			// Example : No Recoil, No Spread
		BF.SyncBFSettingMod();	// Example : UnlockAll

		if (!cfg.Active)
			ExitThread(0);

		Sleep(100); // if you need
	}
}

// Menu
void Menu()
{
    // Create ImGui Window
    ImGui::Begin("BF4 Base", (bool*)NULL, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::BulletText("GunMod");

    ImGui::Checkbox("No Recoil & Spread", &cfg.GunSway);

    ImGui::NewLine();

    ImGui::BulletText("ESP");
    ImGui::Checkbox("ESP", &cfg.ESP);

    ImGui::NewLine();
    ImGui::ColorEdit4("ESP Color (Visible)", &ESP_Visible.x);
    ImGui::ColorEdit4("ESP Color (Not Visible)", &ESP_Normal.x);

    ImGui::End();
}

// ESP
void ESP()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(1920, 1080)); // ToDo : Set your screen size
    ImGui::Begin("##ESP", (bool*)NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    // オーバーレイのフレームレートを取得
    ImGui::Text("%.1f FPS (%.2f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

    // Read Player Data
    DWORD_PTR ClientGameContext = 0;
    DWORD_PTR PlayerManager = 0;
    DWORD_PTR Player = 0;
    DWORD_PTR MemPlayer = 0;
    DWORD_PTR p_tmp = 0;
    DWORD_PTR LocalPlayer = 0;
    DWORD_PTR lp_tmp = 0;
    int LocalPlayerTeam = 0;
    D3DXVECTOR3 LocalPlayerPos = { 0, 0, 0 };

    // Main
    ReadProcessMemory(pHandle, (void*)(OFFSET_CLIENTGAMECONTEXT), &ClientGameContext, sizeof(DWORD_PTR), NULL);
    ReadProcessMemory(pHandle, (void*)(ClientGameContext + OFFSET_PLAYERMANAGER), &PlayerManager, sizeof(DWORD_PTR), NULL);
    ReadProcessMemory(pHandle, (void*)(PlayerManager + OFFSET_PLAYER), &Player, sizeof(DWORD_PTR), NULL);

    // Get LocalPlayer Team
    ReadProcessMemory(pHandle, (void*)(PlayerManager + OFFSET_LOCALPLAYER), &LocalPlayer, sizeof(DWORD_PTR), NULL);
    ReadProcessMemory(pHandle, (void*)(LocalPlayer + OFFSET_TEAM), &LocalPlayerTeam, sizeof(int), NULL);
    // Get LocalPlayer Pos (X, Y, Z)
    ReadProcessMemory(pHandle, (void*)(LocalPlayer + 0x14D0), &lp_tmp, sizeof(DWORD_PTR), NULL);
    ReadProcessMemory(pHandle, (void*)(LocalPlayer + 0x490), &lp_tmp, sizeof(DWORD_PTR), NULL);
    ReadProcessMemory(pHandle, (void*)(lp_tmp + 0x30), &LocalPlayerPos, sizeof(D3DXVECTOR3), NULL);

    // ESP Loop
    for (int i = 0; i < 75; i++)
    {
        DWORD_PTR Soldier = 0;
        DWORD_PTR hp_tmp = 0;
        DWORD_PTR pos_tmp = 0;
        D3DXVECTOR3 PlayerScreenPos = { 0, 0, 0 };

        ReadProcessMemory(pHandle, (void*)(Player + (i * 0x08)), &MemPlayer, sizeof(DWORD_PTR), NULL);
        if (!IsValid(MemPlayer))
            continue;

        ReadProcessMemory(pHandle, (void*)(MemPlayer + OFFSET_SOLDIER), &Soldier, sizeof(DWORD_PTR), NULL);
        if (!IsValid(Soldier))
            continue;

        // Player Team
        int PlayerTeam = 0;
        ReadProcessMemory(pHandle, (void*)(MemPlayer + OFFSET_TEAM), &PlayerTeam, sizeof(int), NULL);

        // HP
        float Health = 0.0f;
        ReadProcessMemory(pHandle, (void*)(Soldier + 0x140), &hp_tmp, sizeof(DWORD_PTR), NULL);
        if (!IsValid(hp_tmp))
            continue;
        ReadProcessMemory(pHandle, (void*)(hp_tmp + 0x20), &Health, sizeof(float), NULL);
        if (Health < 0)
            continue;

        // VisCheck
        bool Visible = false;
        ReadProcessMemory(pHandle, (void*)(Soldier + 0x5B1), &Visible, sizeof(bool), NULL);

        // Player Pos
        ReadProcessMemory(pHandle, (void*)(Soldier + 0x490), &pos_tmp, sizeof(DWORD_PTR), NULL);
        if (!IsValid(pos_tmp))
            continue;
        D3DXVECTOR3 PlayerMemPos = {};
        ReadProcessMemory(pHandle, (void*)(pos_tmp + 0x30), &PlayerMemPos, sizeof(D3DXVECTOR3), NULL);

        // World to screen
        WorldToScreen(PlayerMemPos, PlayerScreenPos);

        Visible = !Visible; // Need it
        if (PlayerScreenPos.x == 0 && PlayerScreenPos.y == 0)
            continue;

        // skip team player
        if (PlayerTeam == LocalPlayerTeam)
            continue;

        // if enemy is visible
        if (Visible)
        {
            d.DrawLine(ImVec2(1920 / 2, 1080), ImVec2(PlayerScreenPos.x, PlayerScreenPos.y), ESP_Visible, 1);
        }
        else
        {
            d.DrawLine(ImVec2(1920 / 2, 1080), ImVec2(PlayerScreenPos.x, PlayerScreenPos.y), ESP_Normal, 1);
        }
    }

    ImGui::End();
}

int main()
{
	// BF4のウィンドウを見つける
	GameHwnd = FindWindowA(NULL, "Battlefield 4");
	if (!GameHwnd)
	{
		MessageBox(NULL, TEXT("Battlefield 4 is not found!"), NULL, MB_ICONWARNING | MB_OK);
		return 0;
	}

	// Get PID
	GetWindowThreadProcessId(GameHwnd, &PID);

	// ProcessHandleを取得
	pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (!pHandle)
	{
		MessageBox(NULL, TEXT("Can't get pHandle!"), NULL, MB_ICONWARNING | MB_OK);
		return 0;
	}

	std::cout << "Ready!\n";
	std::cout << "\nPID : " << PID;

	// Run CheatFuncs
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheatFunctions, NULL, 0, NULL);
	
    // Register WindowClass
    WNDCLASSEXA wc = { sizeof(WNDCLASSEXA), 0, WndProc, 0, 0, NULL, NULL, NULL, NULL, NULL, ("B"), NULL };
    RegisterClassExA(&wc);

    // Create Overlay
    HWND hwnd = CreateWindowExA(WS_EX_TOPMOST, wc.lpszClassName, ("Overlay"), WS_POPUP, 0, 0, 1920, 1080, NULL, NULL, wc.hInstance, NULL); // ToDo : Set your screen size
    SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margin);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClassA(wc.lpszClassName, wc.hInstance);
        exit(0);
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // You can load fonts and ImGui style here
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Background color
    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 0.00f);

    // Main loop
    while (cfg.Active)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                cfg.Active = false;
        }
        if (!cfg.Active)
            break;

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // ShowMenu toggle
        static bool m_tmp = false;
        if (IsKeyDown(VK_INSERT) && !m_tmp)
        {
            ShowMenu = !ShowMenu;
            m_tmp = true;
        }
        else if (!IsKeyDown(VK_INSERT) && m_tmp)
        {
            m_tmp = false;
        }

        if (ShowMenu)
        {
            // Change WindowStyle
            SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED);

            // Rendering Menu
            Menu();
        }
        else
        {
            SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED);
        }

        if (cfg.ESP)
            ESP();

        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
        // g_pSwapChain->Present(0, 0);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClassA(wc.lpszClassName, wc.hInstance);

	return 0;
}

// W2S
bool WorldToScreen(const D3DXVECTOR3& WorldPos, D3DXVECTOR3& ScreenPos)
{
    DWORD_PTR GameRenderer = 0;
    DWORD_PTR RenderView = 0;
    DWORD_PTR DXRenderer = 0;
    DWORD_PTR m_pScreen = 0;
    D3DXMATRIXA16 view_x_projection = {};

    ReadProcessMemory(pHandle, (void*)(OFFSET_GAMERENDERER), &GameRenderer, sizeof(DWORD_PTR), NULL);
    if (!IsValid(GameRenderer))
        return false;
    ReadProcessMemory(pHandle, (void*)(GameRenderer + 0x60), &RenderView, sizeof(DWORD_PTR), NULL);
    if (!IsValid(RenderView))
        return false;
    ReadProcessMemory(pHandle, (void*)(RenderView + 0x420), &view_x_projection, sizeof(D3DXMATRIXA16), NULL);
    ReadProcessMemory(pHandle, (void*)(OFFSET_DXRENDERER), &DXRenderer, sizeof(DWORD_PTR), NULL);
    if (!IsValid(DXRenderer))
        return false;
    ReadProcessMemory(pHandle, (void*)(DXRenderer + 0x38), &m_pScreen, sizeof(DWORD_PTR), NULL);
    if (!IsValid(m_pScreen))
        return false;

    int ScreenWidth = 0.f;
    int ScreenHeight = 0.f;
    ReadProcessMemory(pHandle, (void*)(m_pScreen + 0x58), &ScreenWidth, sizeof(int), NULL);
    ReadProcessMemory(pHandle, (void*)(m_pScreen + 0x5C), &ScreenHeight, sizeof(int), NULL);

    float cX = ScreenWidth * 0.5f;
    float cY = ScreenHeight * 0.5f;

    float w = view_x_projection(0, 3) * WorldPos.x + view_x_projection(1, 3) * WorldPos.y + view_x_projection(2, 3) * WorldPos.z + view_x_projection(3, 3);

    if (w < 0.65f)
    {
        ScreenPos.z = w;
        return false;
    }

    float x = view_x_projection(0, 0) * WorldPos.x + view_x_projection(1, 0) * WorldPos.y + view_x_projection(2, 0) * WorldPos.z + view_x_projection(3, 0);
    float y = view_x_projection(0, 1) * WorldPos.x + view_x_projection(1, 1) * WorldPos.y + view_x_projection(2, 1) * WorldPos.z + view_x_projection(3, 1);

    ScreenPos.x = cX + cX * x / w;
    ScreenPos.y = cY - cY * y / w;
    ScreenPos.z = w;

    return true;
}

bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        cfg.Active = false;
        return 0;
    }
    return DefWindowProcA(hWnd, msg, wParam, lParam);
}
