#include "pch.h"
#include "GameManager.h"
#include "Scenes/DefaultScene.h"
#include "SceneManager.h"
#include "Camera.h"
#include "ConsoleWindow.h"

GameManager* GameManager::Instance = nullptr;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	bool imguiHandled = ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam);

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
	{
		if (wParam != SIZE_MINIMIZED)
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);

			RENDERER.Resize(width, height);
			ConsoleWindow::GetInstance().RequestResize();
		}
		break;
	}
	default:
		if (imguiHandled)
			return true;

		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

GameManager::GameManager()
{
	Instance = this;
}

GameManager::~GameManager()
{
	ReleaseAll();
}

void GameManager::Init(HINSTANCE hInstance)
{
	Initwindow(hInstance);

	Renderer& renderer = RENDERER;
	renderer.Create(m_mainWindow);
	renderer.CreateShader();

	InitImgui();

	SCENE.AddScene("Default", new DefaultScene());
	SCENE.ChangeScene("Default");
	SCENE.Initialize();
}

void GameManager::Initwindow(HINSTANCE hInstance)
{
	WCHAR WindowClass[] = L"3DGameEngine";
	WCHAR Title[] = L"3DGameEngine";

	WNDCLASSW wndclass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, WindowClass };
	RegisterClassW(&wndclass);

	m_mainWindow = CreateWindowExW(0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WIN_WIDTH, WIN_HEIGHT,
		nullptr, nullptr, wndclass.hInstance, nullptr);

	if (m_mainWindow)
	{
		ShowWindow(m_mainWindow, SW_SHOW);
		UpdateWindow(m_mainWindow);
	}
}

void GameManager::InitImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init((void*)m_mainWindow);
	ImGui_ImplDX11_Init(RENDERER.Device, RENDERER.DeviceContext);
}

void GameManager::mainLoop()
{
	// 임구이 프레임 시작
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 게임 로직 업데이트
	Update();

	// 렌더링 실행
	Render();
}

void GameManager::Update()
{
	CAMERA.Update();
	SCENE.Update(DELTA);
}

void GameManager::Render()
{
	Renderer& renderer = RENDERER;

	// 프레임 버퍼 클리어 및 뷰포트 설정
	renderer.Prepare();

	ConsoleWindow::GetInstance().DrawConsole();

	renderer.UpdateFrameConstant();
	CAMERA.SetVPBuffer();

	// 씬 오브젝트 렌더링
	SCENE.Render();

	// 임구이 렌더링
	ImGui::Render();

	ImDrawData* drawData = ImGui::GetDrawData();
	if (drawData)
	{
		ImGui_ImplDX11_RenderDrawData(drawData);
	}

	// 스왑체인 버퍼 교체
	renderer.SwapBuffer();
}

void GameManager::ReleaseAll()
{
	// 씬 리소스 해제
	SCENE.Release();

	// 모든 오브젝트 해제
	OBJECT.DestroyAllObjects();
	OBJECT.DestroyAllMeshes();

	// 임구이 종료
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// 렌더러 리소스 해제
	Renderer& renderer = RENDERER;
	renderer.ReleaseShader();
	renderer.Release();
}
