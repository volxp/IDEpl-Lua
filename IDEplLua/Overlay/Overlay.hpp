#pragma once
#include <Windows.h>
#include <iostream>
#include <thread>
#include <dwmapi.h>
#include <d3d11.h>
#include <vector>
#include <chrono>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/TextEditor.h>
#define IMGUI_DEFINE_MATH_OPERATORS

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "winmm.lib")
// LIB

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef struct Screen
{
	int X = 0, Y = 0;
	int Width = 0, Height = 0;
	bool operator==(const Screen& Screen)
	{
		return X == Screen.X && Y == Screen.Y && Width == Screen.Width && Height == Screen.Height;
	}
	bool operator!=(const Screen& Screen)
	{
		return !(*this == Screen);
	}
} CScreen;

struct Notification
{
	std::string message;
	std::string title;
	std::chrono::steady_clock::time_point startTime;
	float duration;
	bool isVisible;

	Notification(const std::string& msg, const std::string& t, float dur = 4.0f)
		: message(msg), title(t), duration(dur), isVisible(true)
	{
		startTime = std::chrono::steady_clock::now();
	}

	bool IsExpired() const
	{
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count() / 1000.0f;
		return elapsed >= duration;
	}

	float GetAlpha() const
	{
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count() / 1000.0f;
		if (elapsed < 0.3f)
			return elapsed / 0.3f;

		if (elapsed > duration - 0.5f)
			return (duration - elapsed) / 0.5f;

		return 1.0f;
	}
};
struct RBX;

class COverlayHook
{
public:
	ID3D11Device* D3D11Device = nullptr;
	ID3D11DeviceContext* D3D11DeviceContext = nullptr;
	IDXGISwapChain* DxgiSwapChain = nullptr;
	ID3D11RenderTargetView* D3D11RenderTargetView = nullptr;

private:
	std::vector<Notification> notifications;

public:
	bool CreateD3DDevice(HWND handle);
	void CleanupD3DDevice();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	void NotifyImGui(std::string message, std::string title);
	void RenderNotifications();

public:
	bool Initialize(RBX);
};

inline auto OverlayHook = std::make_unique<COverlayHook>();