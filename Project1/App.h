#pragma once

#include "FVertexSimple.h"
#include "Renderer.h"

class App
{
private:
	HWND m_mainWindow = nullptr;

	void Initwindow(HINSTANCE hInstance);
	void InitImgui();

	void Update();
	void Render();

public:
	App();
	~App();

	static App* Instance;

	void Init(HINSTANCE hInstance);
	void mainLoop();
	void ReleaseAll();

	HWND GetMainWindow() const { return m_mainWindow; }
	ID3D11Device* GetDevice() const { return Renderer::GetInstance().Device; }
	ID3D11DeviceContext* GetDeviceContext() const { return Renderer::GetInstance().DeviceContext; }


	//메모리 관리
	uint32 TotalAllocationBytes = 0;
	uint32 TotalAllocationCount = 0;

};