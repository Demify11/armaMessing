#include "Framework.h"
#include "IconsFontAwesome5.h"


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


	{
		Tab& aim = m_form.AddTab(ICON_FA_CROSSHAIRS, "Aim");
		Group& general = aim.AddGroup("Aimbot");
		general.SetFullWidth();
		general.Add<Checkbox>("Aimbot", &bAimBot);
		//general.Add<Button>("Reset", [] { g_Fov = 90.f; });
		general.Add<Button>("Notify", [] {
			ImTricks::NotifyManager::AddNotify("Notify Test", ImTrickNotify_Success);});
		general.Add<SliderFloat>("FOV", &g_Fov, 0.0f, 120.0f);
		Group& WeaponS = aim.AddGroup("Weapon Settings");
		WeaponS.Add<Checkbox>("No Sway", &bNoSway);
		WeaponS.Add<Checkbox>("No Recoil", &bNoRecoil);

		Tab& visuals = m_form.AddTab(ICON_FA_EYE, "Visuals");
		Group& ESP = visuals.AddGroup("ESP");
		ESP.Add<Checkbox>("Enable ESP",&bEsp );
		ESP.Add<Checkbox>("Show Names", &bShowNames);
		ESP.Add<Checkbox>("Show Distance", &bShowDistance);
		
		
		//FOV.Add<RectFilled>();
		Tab& misc = m_form.AddTab("M", "Misc");

	}

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
	ImGuiStyle& Style = ImGui::GetStyle();
	auto& Colors = Style.Colors;
	//io.Fonts->AddFontDefault();
	LoadFonts();


	ImVec4* colors = ImGui::GetStyle().Colors;

	//ImGui::StyleColorsDark();
	CustomColors(&Style);

	ImGui::GetIO().IniFilename = NULL;
	ImGui::GetIO().IniSavingRate = FLT_MAX;
	ImGui::GetIO().WantSaveIniSettings = false;

	ImGui_ImplWin32_Init(m_Window);
	ImGui_ImplDX11_Init(m_Device, m_Context);

	return true;
}

void Overlay::CustomColors(ImGuiStyle* dst) {

	ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
	ImVec4* c = style->Colors;
	c[ImGuiCol_Text] = ImVec4(0.86f, 0.86f, 0.88f, 1.00f);
	c[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.55f, 1.00f);
	c[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 0);
	c[ImGuiCol_ChildBg] = ImVec4(0, 0, 0, 0);// cards show through keep 0
	c[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.12f, 0.98f); // maby for combo

	// --- spacing & rounding (shape stock layout + any remaining widgets) ---
	style->WindowPadding = ImVec2(0, 0);   // load-bearing: step-2 coord alignment
	style->ItemSpacing = ImVec2(10, 10);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->FramePadding = ImVec2(10, 6);
	style->WindowRounding = 10.f;
	style->ChildRounding = 10.f;
	style->FrameRounding = 6.f;
	style->PopupRounding = 5.f;
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

	if (g_shutdown.exchange(false)) {
		g_session.reset();   // joins the thread, best-effort /kill on the process
		
		TerminateProcess(GetCurrentProcess(),0);; //make a proper way to exit process, dont rlly want to call getcurrentprocess();
	}

	if (m_ShowMenu) {

		
		m_form.Draw();
	}
	//---------------------------------------------------------------------------------------------------------


	Entity* TargetEntity = nullptr;

	//-------------------------------------------------------------------------------------------------------

	g_Client->GetWorld()->GetCamera()->Cache(false);

	if (bNoRecoil) {
		noRecoil(ModuleBase);//cache this
	}
	if (bNoSway) {
		noSway(ModuleBase);
	}
	if (bAimBot) {
		
		static float time = 0.0f;

		if (GetAsyncKeyState(VK_LCONTROL)) {
			//This function now returns pointer, instead of copy Its better and gives no errors
			//since we now use UniquePtr which is not copyable.
			TargetEntity = bestTarget(
				g_Client->m_World.m_EntityManager.GetEntities(),
				g_Client->m_World.m_EntityManager.GetVehicles(),
				ModuleBase);

			if (TargetEntity != g_AimSmoother.PrevTarget) {
				g_AimSmoother.havePrev = false;   // new target, no velocity history
				g_AimSmoother.yawVel = 0.f;       // also clear filter momentum so it
				g_AimSmoother.pitchVel = 0.f;     // eases in cleanly instead of jumping
			}
			g_AimSmoother.PrevTarget = TargetEntity;
			
			static const float TimeToTarget = 1.0f;

			time = time <= TimeToTarget ? time + ImGui::GetIO().DeltaTime : time;
			

			if (TargetEntity) {
					

				//best target used to return an entity, which is better, but was changed to return an vector3 because of object slicing
				// the function would return an entity which would cut off all the info for the vehicle.

				//CameraOn.Cache(true);
				auto Angles = CalculateAngles(g_Client->m_World.GetCamera()->CachedViewPosition, TargetEntity->m_HeadPos, g_Client->m_World.m_LocalPlayer.GGunAngles);

				float BulletSpeed = g_Client->m_World.m_LocalPlayer.m_weapon.m_InitSpeed;
				if (BulletSpeed == 0.0f)
					BulletSpeed = g_Client->m_World.m_LocalPlayer.m_weapon.m_Mag.m_MagazineSpeed;

				auto ImprovedAngles = g_Client->m_World.m_Prediction.LeadPrediction(
					TargetEntity->GetHeadPos(),
					TargetEntity->GetVelocity(),
					g_Client->m_World.m_LocalPlayer.m_Velocity,
					g_Client->m_World.m_LocalPlayer.m_HeadPos,
					BulletSpeed,
					g_Client->m_World.m_LocalPlayer.m_weapon.m_Mag.m_AirFriction,
					9.8f, 0.002f, 5.0f, g_Client->m_World.m_LocalPlayer.m_weapon.m_Zeroing);

				//---
				const float kSmoothTime = 0.05f;
				float dt = ImGui::GetIO().DeltaTime;
				Vector3 aimVel = { 0,0,0 };
				if (g_AimSmoother.havePrev && dt > 0.f) {
					aimVel = (ImprovedAngles - g_AimSmoother.prevAimPoint) / dt;
				}
				g_AimSmoother.prevAimPoint = ImprovedAngles;
				g_AimSmoother.havePrev = true;

				Vector3 leadPoint = ImprovedAngles + aimVel * kSmoothTime; //must be the same as in smoothed

				auto NewAngles = CalculateAngles(g_Client->m_World.GetCamera()->CachedViewPosition, leadPoint, g_Client->m_World.m_LocalPlayer.GGunAngles);

				auto Front = g_Client->m_World.GetCamera()->CachedViewAside;
				
				//auto SmoothingAngles = SmoothingUnderdamped(Vector3(Front.x, g_Client->m_World.m_LocalPlayer.GetPitch(), Front.z), NewAngles, time, TimeToTarget);

				Vector3 current = { Front.x, g_Client->m_World.m_LocalPlayer.GetPitch(),Front.z };
				Vector3 smoothed = g_AimSmoother.Update(current, NewAngles, dt, kSmoothTime);

				if (TargetEntity->GetHeadPos() != Vector3(0, 0, 0)) {
					g_Client->m_World.m_LocalPlayer.WriteViewAngles(smoothed);
				}
			}
			else {
				g_AimSmoother.havePrev = false;
			}
		}
		else {
			time = 0.0f;
		}

	}
	if (bHESP)
		HeadESP(g_Client->m_World.m_EntityManager.GetEntities(), WorldAddr, g_Client->m_World.m_EntityManager.GetVehicles());

	if (bEsp)
		ESP(g_Client->m_World.m_EntityManager.GetEntities(), g_Client->m_World.m_EntityManager.GetVehicles(), WorldAddr, ModuleBase);

	char Text[100];

	sprintf_s(Text, "FPS: %.1f", ImGui::GetIO().Framerate);

	ImGui::GetBackgroundDrawList()->AddText(
		ImVec2(50, 50),
		ImColor(255, 255, 255, 255),
		Text
	);

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