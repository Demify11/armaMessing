#pragma once

#undef CreateWindow

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Overlay {
private:
	UINT m_PosX		= 0;
	UINT m_PosY		= 0;
	UINT m_Width	= 0;
	UINT m_Height	= 0;	

	BOOL m_Clickable = FALSE;
	BOOL m_ShowMenu = TRUE;

private:
	WNDCLASSEXW m_WindowClass;

	std::wstring m_WindowName = L"Overlay";
	std::wstring m_ClassName  = L"Burger";

	HWND m_Window = 0;

public:
	ID3D11RenderTargetView* m_Target;
	ID3D11DeviceContext*	m_Context;
	IDXGISwapChain*			m_SwapChain;
	ID3D11Device*			m_Device;

public:
	Overlay();
	~Overlay();

public:
	auto ChangeClickableState(bool State) -> void;
	auto GetClickableState() -> bool;

public:
	bool Init();

private:
	bool PreInit();
	bool CreateWndClass();
	bool CreateWindow();
	bool InitDirectX();
	bool InitImGui();
	bool InitDim();

public:
	void Draw();
	void Loop();

public:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
};

extern Overlay* g_Overlay;