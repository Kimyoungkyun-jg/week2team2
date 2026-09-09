#pragma once

#include "FVertexSimple.h"
#include "FVector.h"
#include "enums.h"
#include "FConstants.h"
#include "Matrix.h"
#include "Containers.h"
#include "GlobalBuffer.h"

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
	ID3D11VertexShader* OutlineVertexShader = nullptr;
	ID3D11VertexShader* SkyVertexShader = nullptr;
	ID3D11PixelShader* SkyPixelShader = nullptr;

	// 화면 및 뷰포트 정보
	D3D11_VIEWPORT ViewportInfo;
	FLOAT ClearColor[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float wAspectRatio;
	float GetAspectRatio() { return wAspectRatio; }

	// 깊이 스텐실 버퍼
	ID3D11DepthStencilView* depthStencilView = nullptr;
	ID3D11DepthStencilState* dsState = nullptr;
	ID3D11DepthStencilState* dsGizmoState = nullptr;
	ID3D11DepthStencilState* dsSelectedState = nullptr;
	ID3D11DepthStencilState* dsOutlineState = nullptr;
	ID3D11DepthStencilState* dsSkyState = nullptr;

	// 단일 공유 컬러 버퍼 (b2 슬롯)
	ColorBuffer* CustomColorBuffer = nullptr;

	FMatrix viewMatrix = FMatrix::Identity();
	FMatrix projMatrix = FMatrix::Identity();

public:
	void Create(HWND hWindow);
	void Release();

	// 깊이 상태 전환
	void SetDefaultDepthState();
	void SetGizmoDepthState();
	void SetSelectedState();
	void SetOutlineState();
	void SetSkyDepthState();
	void SetOutlineParams(float pixels);

	// 선택 액터 아웃라인 렌더링
	void DrawOutline(class AActor* targetActor);


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

	// Color 버퍼 관련
	void CreateColorBuffer();
	void ReleaseColorBuffer();
	void SetCustomColor(const struct FLinearColor& color);

	// 텍스처 및 샘플러 관리
	void CreateSamplerState();
	void ReleaseSamplerState();
	void ReleaseTextures();
	ID3D11ShaderResourceView* LoadTexture(const std::wstring& filePath);
	void SetTexture(ID3D11ShaderResourceView* srv);
	void SetTexture(const std::wstring& filePath);

	ID3D11Buffer* CreateVertexBuffer(const void* vertices, UINT byteWidth);

	void Prepare();

	// 입력 레이아웃과 셰이더 및 파이프라인 준비
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

		// 기본 상태는 커스텀 색상 및 텍스처 미사용
		SetCustomColor({ 0.0f, 0.0f, 0.0f, 0.0f });
		SetTexture(nullptr);
	}
	
	void PrepareOutlineShader(ID3D11InputLayout* layout = nullptr);
	void PrepareSkyShader(ID3D11InputLayout* layout = nullptr);


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

	//창 크기 변경 시 조절
	void Resize(UINT width, UINT height);

private:
	// 타입 식별자 기반 입력 레이아웃 맵
	TMap<const void*, ID3D11InputLayout*> InputLayoutMap;

	// 현재 파이프라인에 바인딩된 입력 레이아웃 캐시
	ID3D11InputLayout* CurrentInputLayout = nullptr;

	// 텍스처 및 샘플러 리소스
	ID3D11SamplerState* SamplerState = nullptr;
	TMap<std::wstring, ID3D11ShaderResourceView*> TextureMap;
	ID3D11ShaderResourceView* CurrentTextureSRV = nullptr;
};
