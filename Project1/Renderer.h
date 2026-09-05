#pragma once

#include "FVertexSimple.h"
#include "FVector.h"
#include "enums.h"
#include "FConstants.h"
#include "Matrix.h"



struct FVertexBufferInfo
{
	ID3D11Buffer* vertexBuffer;
	UINT numVertucies;
};

class Renderer
{


public:
	static Renderer& GetInstance()
	{
		static Renderer instance;
		return instance;
	}

	ID3D11Device* GetDevice() { return Device; }
	ID3D11DeviceContext* GetDeviceContext() { return DeviceContext; }

public:
	// CreateDeviceAndSwapChain
	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* DeviceContext = nullptr;
	IDXGISwapChain* SwapChain = nullptr;

	// CreateFrameBuffer
	ID3D11Texture2D* FrameBuffer = nullptr;
	ID3D11RenderTargetView* FrameBufferRTV = nullptr;

	// CreateRasterizerState
	ID3D11RasterizerState* RasterizerState = nullptr;

	// CreateShader
	ID3D11VertexShader* SimpleVertexShader = nullptr;
	ID3D11PixelShader* SimplePixelShader = nullptr;
	ID3D11InputLayout* SimpleInputLayout = nullptr;


	// Direct2D & WIC Management
	ID2D1Factory* D2DFactory = nullptr;
	ID2D1RenderTarget* D2DRenderTarget = nullptr;
	IDWriteFactory* DWriteFactory = nullptr;
	IWICImagingFactory* WICFactory = nullptr;

	// values
	D3D11_VIEWPORT ViewportInfo;
	FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f };
	unsigned int Stride;
	float wAspectRatio;
	float GetAspectRatio() { return wAspectRatio; }

	// 깊이 스텐실 버퍼
	ID3D11DepthStencilView* depthStencilView = nullptr;
	ID3D11DepthStencilState* dsState = nullptr;


	FMatrix viewMatrix = FMatrix::Identity();
	FMatrix projMatrix = FMatrix::Identity();


public:
	void Create(HWND hWindow);
	void Release();


	void CreateDeviceAndSwapChain(HWND hWindow);
	void ReleaseDeviceAndSwapChain();

	void CreateFrameBuffer();
	void ReleaseFrameBuffer();

	void CreateRasterizerState();
	void ReleaseRasterizerState();

	void CreateShader();
	void ReleaseShader();


	void CreateVertexBufferInfos();
	ID3D11Buffer* CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth);
	void ReleaseVertexBuffers();

	void Prepare();
	void PrepareShader();

	
	void UpdateConstant(FVector Offset, FVector Scale);
	void UpdateFrameConstant();
	void Update();

	

	//버텍스 버퍼 세팅
	void SetVSBuffer(UINT slot);

	//깊이 버퍼 세팅
	void CreateDepthStencil();
	void ReleaseDepthStencil();

	void RenderPrimitive(EPrimitive Primitive);
	void SwapBuffer();

private:
	void ReleaseVertexBuffer(ID3D11Buffer* vertexBuffer);
	TArray<FVertexBufferInfo> VertexBufferInfos;

};
