#pragma once
#include "FVertexSimple.h"
#include "Renderer.h"
#include "ConsoleWindow.h"

class GameManager
{
private:
	HWND m_mainWindow = nullptr;
	
	void Initwindow(HINSTANCE hInstance);
	void InitImgui();

	void Update();
	void Render();
	
public:
	GameManager();
	~GameManager();

	static GameManager* Instance;
	static GameManager& GetInstance() { return *Instance; }

	void Init(HINSTANCE hInstance);
	void mainLoop();
	void ReleaseAll();

	HWND GetMainWindow() const { return m_mainWindow; }
	ID3D11Device* GetDevice() const { return RENDERER.Device; }
	ID3D11DeviceContext* GetDeviceContext() const { return RENDERER.DeviceContext; }
};

// 기존 이름 호환 별칭
using App = GameManager;
