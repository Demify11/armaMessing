#include "Framework.h"


UINT64 ModuleBase;
UINT64 WorldAddr;

Overlay::Overlay() {

}

Overlay::~Overlay() {
	UnregisterClass(m_WindowClass.lpszClassName, m_WindowClass.hInstance);
}

auto Overlay::ChangeClickableState(bool State) -> void {
	m_Clickable = (BOOL)State;

	if (State) 
		SetWindowLong(m_Window, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
	else
		SetWindowLong(m_Window, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT);
}

auto Overlay::GetClickableState() -> bool {
	return (bool)m_Clickable;
}


bool Overlay::Init() {

	if (!PreInit())
		return false;

	if (!InitDim())
		return false;

	if (!CreateWndClass())
		return false;

	if (!CreateWindow())
		return false;

	if (!InitDirectX())
		return false;

	if (!InitImGui())
		return false;

    return true;
}

bool Overlay::PreInit() {
	return (bool)SetProcessDPIAware();
}

bool Overlay::CreateWndClass() {

	m_WindowClass.cbSize        = sizeof(m_WindowClass);
	m_WindowClass.style         = CS_CLASSDC;
    m_WindowClass.lpfnWndProc   = Overlay::WindowProc;
    m_WindowClass.cbClsExtra    = 0L;
    m_WindowClass.cbWndExtra    = 0L;
    m_WindowClass.hInstance     = GetModuleHandle(NULL);
    m_WindowClass.hIcon         = NULL;
    m_WindowClass.hCursor       = NULL;
    m_WindowClass.hbrBackground = NULL;
    m_WindowClass.lpszMenuName  = NULL;
    m_WindowClass.lpszClassName = m_ClassName.c_str();
    m_WindowClass.hIconSm       = NULL;
	
    ::RegisterClassExW(&m_WindowClass);

	return true;
}

bool Overlay::CreateWindow() {
	m_Window = CreateWindowEx(
		WS_EX_LAYERED
		| WS_EX_TOOLWINDOW
		| WS_EX_TOPMOST
		| WS_EX_TRANSPARENT,
		m_WindowClass.lpszClassName,
		m_WindowName.c_str(),
		WS_POPUP, m_PosX, m_PosY,
		m_Width,
		m_Height,
		0,
		HMENU(),
		GetModuleHandle(NULL),
		NULL
	);

	if (!m_Window)
		return false;

	if (!SetLayeredWindowAttributes(m_Window, 0, 255, LWA_ALPHA))
		return false;

	MARGINS Margin = { -1 };

	if (FAILED(DwmExtendFrameIntoClientArea(m_Window, &Margin)))
		return false;

	return true;
}

bool Overlay::InitDirectX() {

	DXGI_SWAP_CHAIN_DESC SwapChainDescriptor;
	ZeroMemory(&SwapChainDescriptor, sizeof(SwapChainDescriptor));

	SwapChainDescriptor.BufferCount = 2;
	SwapChainDescriptor.BufferDesc.Width = 0;
	SwapChainDescriptor.BufferDesc.Height = 0;
	SwapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDescriptor.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDescriptor.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDescriptor.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	SwapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDescriptor.OutputWindow = m_Window;
	SwapChainDescriptor.SampleDesc.Count = 1;
	SwapChainDescriptor.SampleDesc.Quality = 0;
	SwapChainDescriptor.Windowed = TRUE;
	SwapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel;

	const D3D_FEATURE_LEVEL featureLevelArray[2] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
	};

	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &SwapChainDescriptor, &m_SwapChain, &m_Device, &featureLevel, &m_Context) != S_OK)
		return false;

	ID3D11Texture2D* BackBuffer;

	if (FAILED(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&BackBuffer))))
		return false;

	if (FAILED(m_Device->CreateRenderTargetView(BackBuffer, NULL, &m_Target)))
		return false;

	BackBuffer->Release();

	return true;
}

bool Overlay::InitImGui() {
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.Fonts->AddFontDefault();

	ImGui::StyleColorsDark();

	ImGui::GetIO().IniFilename = NULL;
	ImGui::GetIO().IniSavingRate = FLT_MAX;
	ImGui::GetIO().WantSaveIniSettings = false;

	ImGui_ImplWin32_Init(m_Window);
	ImGui_ImplDX11_Init(m_Device, m_Context);

	return true;
}

bool Overlay::InitDim() {
	m_PosX = 0;
	m_PosY = 0;

	m_Width = GetSystemMetrics(SM_CXSCREEN);
	m_Height = GetSystemMetrics(SM_CYSCREEN);

	return m_Width && m_Height;
}

void Overlay::Draw() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_Once);
	static bool bNoRecoil = false;
	static bool bNoSway = false;
	static bool bAimBot = false;
	static bool bHESP = false;
	static bool bEsp = false;

	if (m_ShowMenu) {

		// Put this in it's own function, or else.
		ImGui::SetNextWindowSize(ImVec2(600, 400));
		ImGui::Begin("Who Was In Paris",NULL,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		ImGui::Checkbox("AimBot", &bAimBot);

		ImGui::Checkbox("Head Esp", &bHESP);

		ImGui::Checkbox("ESP", &bEsp);

		ImGui::Checkbox("No Sway", &bNoSway);
		
		
		ImGui::Checkbox("No Recoil", &bNoRecoil);

		ImGui::End();

	}
	//---------------------------------------------------------------------------------------------------------


	Vector3 TargetEntity;
	// You would normally have some form of target selection.
	// So you'd have a list of entities, and then sort.
	// First you check if they are invis
	// - then dead
	// - then far away (like 2000m +)
	// - then if they're in fov
	// if all of these criterias are hit, you get the one closest to the crosshair / center.
	// and then you do aimbot on that cunt.
	// you can calculate that yourself.
	// you already have everything needed
	// 
	//TargetEntity = Entities[2];
	//auto Angles = CalculateAngles(CameraOn.GetHeadPosition2(ModuleBase), TargetEntity.GetHeadPosition(), CameraOn.GetGunAngles());

	//auto LocalPlayer1 = GetLocalPlayer(WorldAddr);


	
	if (GetAsyncKeyState(VK_RBUTTON)) {
		TargetEntity = bestTarget(g_Client->GetWorld()->GetEntities(), g_Client->GetWorld()->GetVehicles() , ModuleBase);
		//best target used to return an entity, which is better, but was changed to return an vector3 because of object slicing
		// the function would return an entity which would cut off all the info for the vehicle.
		
		//CameraOn.Cache(true);
		auto Angles = CalculateAngles(g_Client->m_World.GetCamera()->CachedViewPosition, TargetEntity, g_Client->m_World.m_LocalPlayer.GGunAngles);
		
		if (TargetEntity != Vector3(0,0,0)) {

			g_Client->m_World.m_LocalPlayer.WriteViewAngles(Angles);
		}
	}
	
	



	// 200 meter distance lol.
	// 20 y 20 grid
	// remember arma works with a xz-plane







	//-------------------------------------------------------------------------------------------------------

	if (bNoRecoil) {
		noRecoil(ModuleBase);//cache this
	}
	if (bNoSway) {
		noSway(ModuleBase);
	}
	if (bAimBot) {

	}
	if (bHESP)
		HeadESP(g_Client->GetWorld()->GetEntities(), WorldAddr, g_Client->GetWorld()->GetVehicles());

	if (bEsp)
	ESP(g_Client->m_World.entityCache, WorldAddr, ModuleBase);

	ImGui::EndFrame();
	ImGui::Render();

	constexpr float ClearColorWithAlpha[4] = { 0, 0, 0, 0 };

	m_Context->OMSetRenderTargets(1, &m_Target, NULL);
	m_Context->ClearRenderTargetView(m_Target, ClearColorWithAlpha);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	m_SwapChain->Present(1, 0); // VSYNC
}

void Overlay::Loop() {

	::ShowWindow(m_Window, SW_SHOW);
	ChangeClickableState(m_ShowMenu);

	while (TRUE) {
		MSG uMessage;

		while (::PeekMessageW(&uMessage, NULL, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&uMessage);
			::DispatchMessageW(&uMessage);

			if (uMessage.message == WM_QUIT)
				break;
		}

		if (GetAsyncKeyState(VK_INSERT)) {

			m_ShowMenu = !m_ShowMenu;

			ChangeClickableState(m_ShowMenu);

			while (GetAsyncKeyState(VK_INSERT)) {}
		}


		if (GetAsyncKeyState(VK_END))
			break;

		Draw();

	}

	exit(1);
}

LRESULT CALLBACK Overlay::WindowProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMessage, wParam, lParam))
		return true;

	return ::DefWindowProcW(hWnd, uMessage, wParam, lParam);
}