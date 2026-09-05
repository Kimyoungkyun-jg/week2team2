#include "pch.h"
#include "App.h"
#include "Scenes/DefaultScene.h"
#include "SceneManager.h"
#include "Camera.h"

App* App::Instance = nullptr;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	bool imguiHandled = ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam);

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		if (imguiHandled)
			return true;

		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

App::App()
{
	Instance = this;
}

App::~App()
{
	ReleaseAll();
}

void App::Init(HINSTANCE hInstance)
{
	Initwindow(hInstance);

	Renderer& renderer = Renderer::GetInstance();
	renderer.Create(m_mainWindow);
	renderer.CreateShader();
	renderer.CreateConstantBuffer();
	renderer.CreateFrameConstantBuffer();
	renderer.CreateVertexBufferInfos();

	InitImgui();

	SCENE.AddScene("Default", new DefaultScene());
	SCENE.ChangeScene("Default");
	SCENE.Initialize();
}

void App::Initwindow(HINSTANCE hInstance)
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

void App::InitImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init((void*)m_mainWindow);
	ImGui_ImplDX11_Init(Renderer::GetInstance().Device, Renderer::GetInstance().DeviceContext);
}

void App::mainLoop()
{
	Update();
	Render();
}

void App::Update()
{
	Camera::GetInstance().Update();
	SCENE.Update(DELTA);
}

void App::Render()
{
	Renderer& renderer = Renderer::GetInstance();

	// 프레임 버퍼 클리어 및 뷰포트/래스터라이저 설정
	renderer.Prepare();
	// 셰이더 및 상수 버퍼 설정
	renderer.PrepareShader();

	// ImGui 프레임 시작
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// ImGui 테스트 및 데모 윈도우 (상시 출력)
	ImGui::ShowDemoWindow();

	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::Begin("Engine Main Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("DirectX 11 & ImGui Active!");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();

	// 씬 오브젝트 렌더링 (Renderer를 통해 Draw)
	SCENE.Render();

	// ImGui 렌더링
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// 스왑 체인 Present
	renderer.SwapBuffer();
}

void App::ReleaseAll()
{


	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Renderer& renderer = Renderer::GetInstance();
	renderer.ReleaseVertexBuffers();
	renderer.ReleaseFrameConstantBuffer();
	renderer.ReleaseConstantBuffer();
	renderer.ReleaseShader();
	renderer.Release();
}

