#pragma once

#include "FVertexSimple.h"
#include "FVector.h"
#include "enums.h"
#include "FConstants.h"
#include "Matrix.h"
#include "Containers.h"
#include <string_view>

class UObject;
struct ClassInfo;

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

	// CreateShader 헬퍼 함수들
	bool CreateVertexShader(LPCWSTR path, LPCSTR entryPoint, ID3D11VertexShader** outVS, ID3DBlob** outBlob = nullptr);
	bool CreatePixelShader(LPCWSTR path, LPCSTR entryPoint, ID3D11PixelShader** outPS);
	bool CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements, ID3DBlob* vsBlob, ID3D11InputLayout** outLayout);

	void CreateShader();
	void ReleaseShader();

	//Color관련
	void CreateColorBuffer();



	ID3D11Buffer* CreateVertexBuffer(const void* vertices, UINT byteWidth);

	void Prepare();

	// InputLayout 포인터(또는 기본 레이아웃)로 셰이더 및 파이프라인 세팅
	void PrepareShader(ID3D11InputLayout* layout = nullptr)
	{
		ID3D11InputLayout* targetLayout = layout ? layout : SimpleInputLayout;
		if (CurrentInputLayout != targetLayout)
		{
			CurrentInputLayout = targetLayout;
			DeviceContext->IASetInputLayout(targetLayout);
		}
		DeviceContext->VSSetShader(SimpleVertexShader, nullptr, 0);
		DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
	}

	// [핵심] 타입만 넘기면 FVertexTraits를 통해 자동으로 InputLayout을 생성하고 TMap에 등록하는 템플릿 함수
	template<typename VertexType>
	ID3D11InputLayout* RegisterInputLayout(ID3DBlob* vsBlob)
	{
		if (!vsBlob)
			return nullptr;

		const void* typeKey = &typeid(VertexType);
		auto it = InputLayoutMap.find(typeKey);
		if (it != InputLayoutMap.end())
			return it->second;

		ID3D11InputLayout* layout = nullptr;
		HRESULT hr = Device->CreateInputLayout(
			FVertexLayouts<VertexType>::Layout,
			FVertexLayouts<VertexType>::NumElements,
			vsBlob->GetBufferPointer(),
			vsBlob->GetBufferSize(),
			&layout);

		if (SUCCEEDED(hr) && layout)
		{
			InputLayoutMap[typeKey] = layout;
			if (!SimpleInputLayout)
			{
				SimpleInputLayout = layout;
			}
		}
		return layout;
	}

	// 타입으로 InputLayout 조회
	template<typename VertexType>
	ID3D11InputLayout* GetInputLayout()
	{
		const void* typeKey = &typeid(VertexType);
		auto it = InputLayoutMap.find(typeKey);
		if (it != InputLayoutMap.end())
			return it->second;
		return SimpleInputLayout;
	}

	ID3D11InputLayout* GetInputLayout(const ClassInfo* classInfo);
	ID3D11InputLayout* GetInputLayout(const UObject* object);
	
	void UpdateConstant(FVector Offset, FVector Scale);
	void UpdateFrameConstant();
	void Update();

	//버텍스 버퍼 세팅
	void SetVSBuffer(UINT slot);

	//깊이 버퍼 세팅
	void CreateDepthStencil();
	void ReleaseDepthStencil();

	void SwapBuffer();

private:
	// 타입 식별자 (const void*) 기반 InputLayout TMap
	TMap<const void*, ID3D11InputLayout*> InputLayoutMap;

	// 현재 파이프라인에 바인딩된 InputLayout 캐시 (중복 상태 전환 방지)
	ID3D11InputLayout* CurrentInputLayout = nullptr;

	ID3D11Buffer* ColorBuffer = nullptr;

	
};
