#include "pch.h"
#include "Renderer.h"
#include "Sphere.h"
#include "Camera.h"
#include "UObject.h"
#include "GlobalBuffer.h"
#include "AActor.h"
#include "VertexBuffer.h"
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

void Renderer::Create(HWND hWindow)
{
	CreateDeviceAndSwapChain(hWindow);
	CreateFrameBuffer();
	CreateDepthStencil();
	CreateRasterizerState();
	CreateShader();
	CreateColorBuffer();
	CreateSamplerState();
}

void Renderer::Release()
{
	ReleaseTextures();
	ReleaseSamplerState();
	ReleaseColorBuffer();
	ReleaseDepthStencil();
	ReleaseShader();
	ReleaseRasterizerState();
	ReleaseFrameBuffer();
	ReleaseDeviceAndSwapChain();
}

void Renderer::CreateColorBuffer()
{
	CustomColorBuffer = new ::ColorBuffer();
}

void Renderer::ReleaseColorBuffer()
{
	if (CustomColorBuffer)
	{
		delete CustomColorBuffer;
		CustomColorBuffer = nullptr;
	}
}

void Renderer::SetCustomColor(const FLinearColor& color)
{
	if (CustomColorBuffer)
	{
		CustomColorBuffer->SetColor(color);
		CustomColorBuffer->SetVSBuffer(2);
		CustomColorBuffer->SetPSBuffer(2);
	}
}

void Renderer::CreateSamplerState()
{
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0.0f;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Device->CreateSamplerState(&sampDesc, &SamplerState);
}

void Renderer::ReleaseSamplerState()
{
	if (SamplerState)
	{
		SamplerState->Release();
		SamplerState = nullptr;
	}
}

void Renderer::ReleaseTextures()
{
	for (auto& pair : TextureMap)
	{
		if (pair.second)
		{
			pair.second->Release();
		}
	}
	TextureMap.clear();
	CurrentTextureSRV = nullptr;
}

ID3D11ShaderResourceView* Renderer::LoadTexture(const std::wstring& filePath)
{
	auto it = TextureMap.find(filePath);
	if (it != TextureMap.end())
	{
		return it->second;
	}

	std::filesystem::path p(filePath);
	
	// 실행 디렉토리나 상대 경로에 따른 파일 경로 탐색
	if (!std::filesystem::exists(p))
	{
		std::filesystem::path alt1 = std::filesystem::path(L"Project1") / p;
		std::filesystem::path alt2 = std::filesystem::path(L"../Project1") / p;
		if (std::filesystem::exists(alt1))
		{
			p = alt1;
		}
		else if (std::filesystem::exists(alt2))
		{
			p = alt2;
		}
	}

	std::wstring ext = p.extension().wstring();
	std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);

	DirectX::ScratchImage image;
	HRESULT hr = S_OK;

	if (ext == L".dds")
	{
		hr = DirectX::LoadFromDDSFile(p.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else
	{
		hr = DirectX::LoadFromWICFile(p.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
	}

	if (FAILED(hr))
	{
		return nullptr;
	}

	ID3D11ShaderResourceView* srv = nullptr;
	hr = DirectX::CreateShaderResourceView(
		Device,
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		&srv
	);

	if (FAILED(hr) || !srv)
	{
		return nullptr;
	}

	TextureMap[filePath] = srv;
	return srv;
}

void Renderer::SetTexture(ID3D11ShaderResourceView* srv)
{
	CurrentTextureSRV = srv;
	if (srv)
	{
		DeviceContext->PSSetShaderResources(0, 1, &srv);
		if (CustomColorBuffer)
		{
			CustomColorBuffer->SetUseTexture(1);
			CustomColorBuffer->SetPSBuffer(2);
		}
	}
	else
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		DeviceContext->PSSetShaderResources(0, 1, &nullSRV);
		if (CustomColorBuffer)
		{
			CustomColorBuffer->SetUseTexture(0);
			CustomColorBuffer->SetPSBuffer(2);
		}
	}
}

void Renderer::SetTexture(const std::wstring& filePath)
{
	SetTexture(LoadTexture(filePath));
}

void Renderer::CreateDeviceAndSwapChain(HWND hWindow)
{
	D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

	DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
	swapchaindesc.BufferDesc.Width = 0;
	swapchaindesc.BufferDesc.Height = 0;
	swapchaindesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchaindesc.BufferDesc.RefreshRate.Numerator = 60;
	swapchaindesc.BufferDesc.RefreshRate.Denominator = 1;
	swapchaindesc.SampleDesc.Count = 1;
	swapchaindesc.SampleDesc.Quality = 0;
	swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchaindesc.BufferCount = 1;
	swapchaindesc.OutputWindow = hWindow;
	swapchaindesc.Windowed = TRUE;

	UINT createDeviceFlags = 0;

	#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;   // 디버그 레이어도 같이 켜기
	#endif

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
		featurelevels,
		ARRAYSIZE(featurelevels),
		D3D11_SDK_VERSION,
		&swapchaindesc,
		&SwapChain,
		&Device,
		nullptr,
		&DeviceContext);
}

void Renderer::ReleaseDeviceAndSwapChain()
{
	if (DeviceContext)
	{
		DeviceContext->Release();
		DeviceContext = nullptr;
	}
	if (Device)
	{
		Device->Release();
		Device = nullptr;
	}
	if (SwapChain)
	{
		SwapChain->Release();
		SwapChain = nullptr;
	}
}

void Renderer::CreateFrameBuffer()
{
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&FrameBuffer);
	Device->CreateRenderTargetView(FrameBuffer, nullptr, &FrameBufferRTV);

	D3D11_TEXTURE2D_DESC framebufferdesc = {};
	FrameBuffer->GetDesc(&framebufferdesc);

	ViewportInfo = { 0, 0, (FLOAT)framebufferdesc.Width, (FLOAT)framebufferdesc.Height, 0.0f, 1.0f };

	wAspectRatio = (float)framebufferdesc.Width / (float)framebufferdesc.Height;
}

void Renderer::ReleaseFrameBuffer()
{
	if (FrameBufferRTV)
	{
		FrameBufferRTV->Release();
		FrameBufferRTV = nullptr;
	}
	if (FrameBuffer)
	{
		FrameBuffer->Release();
		FrameBuffer = nullptr;
	}
}

void Renderer::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterizerdesc = {};
	rasterizerdesc.FillMode = D3D11_FILL_SOLID;
	rasterizerdesc.CullMode = D3D11_CULL_BACK;
	rasterizerdesc.FrontCounterClockwise = FALSE;
	rasterizerdesc.DepthClipEnable = TRUE;

	Device->CreateRasterizerState(&rasterizerdesc, &RasterizerState);
}

void Renderer::ReleaseRasterizerState()
{
	if (RasterizerState)
	{
		RasterizerState->Release();
		RasterizerState = nullptr;
	}
}

bool Renderer::CreateVertexShader(LPCWSTR path, LPCSTR entryPoint, ID3D11VertexShader** outVS, ID3DBlob** outBlob)
{
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DCompileFromFile(
		path, nullptr, nullptr,
		entryPoint, "vs_5_0", 0, 0, &vsBlob, &errorBlob);

	if (FAILED(hr) || !vsBlob)
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		return false;
	}

	hr = Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, outVS);

	if (outBlob)
	{
		*outBlob = vsBlob;
	}
	else
	{
		vsBlob->Release();
	}

	return SUCCEEDED(hr);
}

bool Renderer::CreatePixelShader(LPCWSTR path, LPCSTR entryPoint, ID3D11PixelShader** outPS)
{
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DCompileFromFile(
		path, nullptr, nullptr,
		entryPoint, "ps_5_0", 0, 0, &psBlob, &errorBlob);

	if (FAILED(hr) || !psBlob)
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		return false;
	}

	hr = Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, outPS);
	psBlob->Release();

	return SUCCEEDED(hr);
}

bool Renderer::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements, ID3DBlob* vsBlob, ID3D11InputLayout** outLayout)
{
	if (!vsBlob || !outLayout)
		return false;

	HRESULT hr = Device->CreateInputLayout(
		layoutDesc, numElements,
		vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
		outLayout);

	return SUCCEEDED(hr);
}

void Renderer::CreateShader()
{
	LPCWSTR shaderPath = L"ShaderW0.hlsl";
	LPCWSTR GridshaderPath = L"GridShader.hlsl";

	//Vertex & Pixel Shader 컴파일 및 생성
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* gridVSBlob = nullptr;

	CreateVertexShader(shaderPath, "mainVS", &SimpleVertexShader, &vsBlob);
	CreatePixelShader(shaderPath, "mainPS", &SimplePixelShader);
	CreateVertexShader(shaderPath, "mainVS_Outline", &OutlineVertexShader);
	CreateVertexShader(GridshaderPath,"mainVS_Grid",&GridVertexShader,&gridVSBlob);
	CreatePixelShader(GridshaderPath, "mainPS_Grid", &GridPixelShader);
	CreateVertexShader(shaderPath, "mainVS_Sky", &SkyVertexShader);
	CreatePixelShader(shaderPath, "mainPS_Sky", &SkyPixelShader);

	//정점 타입만 넘기면 FVertexTraits를 통해 자동으로 InputLayout을 생성하고 TMap에 등록
	RegisterInputLayout<FVertexSimple>(vsBlob);
	RegisterInputLayout<FVertexColor>(vsBlob);
	RegisterInputLayout<FVertexUV>(vsBlob);


	if (vsBlob)
	{
		vsBlob->Release();
	}
}

void Renderer::ReleaseShader()
{
	for (auto& pair : InputLayoutMap)
	{
		if (pair.second)
		{
			pair.second->Release();
		}
	}

	InputLayoutMap.clear();
	SimpleInputLayout = nullptr;

	if (SkyPixelShader)
	{
		SkyPixelShader->Release();
		SkyPixelShader = nullptr;
	}

	if (SkyVertexShader)
	{
		SkyVertexShader->Release();
		SkyVertexShader = nullptr;
	}

	if (SimplePixelShader)
	{
		SimplePixelShader->Release();
		SimplePixelShader = nullptr;
	}

	if (OutlineVertexShader)
	{
		OutlineVertexShader->Release();
		OutlineVertexShader = nullptr;
	}

	if (SimpleVertexShader)
	{
		SimpleVertexShader->Release();
		SimpleVertexShader = nullptr;
	}
}

ID3D11InputLayout* Renderer::GetInputLayout(const ClassInfo* classInfo)
{
	if (!classInfo)
		return SimpleInputLayout;

	return SimpleInputLayout;
}

ID3D11InputLayout* Renderer::GetInputLayout(const UObject* object)
{
	if (!object)
		return SimpleInputLayout;

	if (object->GetClass())
	{
		return GetInputLayout(object->GetClass());
	}
	return SimpleInputLayout;
}

ID3D11Buffer* Renderer::CreateVertexBuffer(const void* vertices, UINT byteWidth)
{
	D3D11_BUFFER_DESC vertexbufferdesc = {};
	vertexbufferdesc.ByteWidth = byteWidth;
	vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexbufferSRD = { vertices };

	ID3D11Buffer* vertexBuffer;

	Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);

	return vertexBuffer;
}

void Renderer::Prepare()
{
	DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);

	DeviceContext->RSSetViewports(1, &ViewportInfo);
	DeviceContext->RSSetState(RasterizerState);


	DeviceContext->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

	DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, depthStencilView);
	DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);

	UINT stencilRef = 1; // 스텐실 기준값
	DeviceContext->OMSetDepthStencilState(dsState, stencilRef);

	// 샘플러 바인딩 및 텍스처 초기화
	if (SamplerState)
	{
		DeviceContext->PSSetSamplers(0, 1, &SamplerState);
	}
	SetTexture(nullptr);

	// 입력 레이아웃 캐시 초기화
	CurrentInputLayout = nullptr;
}

void Renderer::PrepareOutlineShader(ID3D11InputLayout* layout)
{
	ID3D11InputLayout* targetLayout = layout ? layout : SimpleInputLayout;
	if (CurrentInputLayout != targetLayout) {
		CurrentInputLayout = targetLayout;
		DeviceContext->IASetInputLayout(targetLayout);
	}
	DeviceContext->VSSetShader(OutlineVertexShader, nullptr, 0);
	DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
}

void Renderer::PrepareSkyShader(ID3D11InputLayout* layout)
{
	ID3D11InputLayout* targetLayout = layout ? layout : SimpleInputLayout;
	if (CurrentInputLayout != targetLayout) {
		CurrentInputLayout = targetLayout;
		DeviceContext->IASetInputLayout(targetLayout);
	}
	DeviceContext->VSSetShader(SkyVertexShader, nullptr, 0);
	DeviceContext->PSSetShader(SkyPixelShader, nullptr, 0);
}


void Renderer::UpdateFrameConstant()
{
	Camera& cam = CAMERA;
	cam.vpBuffer->SetMat(cam.GetViewMatrix() * cam.GetProjectionMatrix(wAspectRatio), cam.GetLocation());
}


void Renderer::Update()
{

}


void Renderer::CreateDepthStencil()
{
	{//깊이 버퍼용 텍스쳐 생성
		
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = (UINT)ViewportInfo.Width;
		descDepth.Height = (UINT)ViewportInfo.Height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Depth 24비트, Stencil 8비트
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		ID3D11Texture2D* depthStencilBuffer = nullptr;
		HRESULT hr = Device->CreateTexture2D(&descDepth, nullptr, &depthStencilBuffer);
		if (SUCCEEDED(hr) && depthStencilBuffer)
		{
			Device->CreateDepthStencilView(depthStencilBuffer, nullptr, &depthStencilView);
			depthStencilBuffer->Release();
		}

	}

	{ //기본 
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};

		// 깊이 테스트 설정
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // Z 버퍼 기록 허용
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;     // 가까운 것만 통과

		// 스텐실 테스트 설정
		dsDesc.StencilEnable = FALSE;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;

		// 전면 폴리곤(Front Face) 스텐실 규칙
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE; // 통과 시 Ref 값으로 기록
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;    // 무조건 통과 (마스킹 단계)

		// 후면 폴리곤(Back Face) 스텐실 규칙
		dsDesc.BackFace = dsDesc.FrontFace;

		Device->CreateDepthStencilState(&dsDesc, &dsState);
	}

	{//기즈모용 깊이 스텐실
		//(깊이 테스트 비활성화로 항상 최상단 렌더링, 스텐실 마킹으로 외곽선 침범 방지)
		D3D11_DEPTH_STENCIL_DESC gizmoDesc = {};
		gizmoDesc.DepthEnable = FALSE;
		gizmoDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

		// 기즈모 영역도 스텐실 1로 마킹하여 외곽선이 덮지 못하게 보호
		gizmoDesc.StencilEnable = TRUE;
		gizmoDesc.StencilReadMask = 0xFF;
		gizmoDesc.StencilWriteMask = 0xFF;

		gizmoDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		gizmoDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		gizmoDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		gizmoDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		gizmoDesc.BackFace = gizmoDesc.FrontFace;
		
		Device->CreateDepthStencilState(&gizmoDesc, &dsGizmoState);
	}


	{// 아웃라이너용
		//깊이 스텐실 상태 (1이 아니라면 아웃라이너 그리기)
		D3D11_DEPTH_STENCIL_DESC outlinerDesc = {};
		outlinerDesc.DepthEnable = FALSE;
		outlinerDesc.DepthFunc = D3D11_COMPARISON_LESS;

		outlinerDesc.StencilEnable = TRUE;
		outlinerDesc.StencilReadMask = 0xFF;
		outlinerDesc.StencilWriteMask = 0xFF;

		outlinerDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;	// 통과 조건: 새값!=기존값이면 통과!
		outlinerDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;		// stencil 실패시
		outlinerDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;	// stencil 통과, 깊이 실패
		outlinerDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		// 둘다 통과시

		outlinerDesc.BackFace = outlinerDesc.FrontFace;

		Device->CreateDepthStencilState(&outlinerDesc, &dsOutlineState);
	}

	{// 스카이스피어용 깊이 상태
		
		D3D11_DEPTH_STENCIL_DESC skyDesc = {};
		skyDesc.DepthEnable = TRUE;
		skyDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		skyDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		skyDesc.StencilEnable = FALSE;

		Device->CreateDepthStencilState(&skyDesc, &dsSkyState);
	}

	{// 선택된 액터용 스텐실 마킹 상태
		D3D11_DEPTH_STENCIL_DESC selectedDesc = {};
		selectedDesc.DepthEnable = TRUE;
		selectedDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		selectedDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		selectedDesc.StencilEnable = TRUE;
		selectedDesc.StencilReadMask = 0xFF;
		selectedDesc.StencilWriteMask = 0xFF;

		selectedDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		selectedDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		selectedDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		selectedDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		selectedDesc.BackFace = selectedDesc.FrontFace;

		Device->CreateDepthStencilState(&selectedDesc, &dsSelectedState);
	}

}

void Renderer::ReleaseDepthStencil()
{
	if (dsSelectedState)
	{
		dsSelectedState->Release();
		dsSelectedState = nullptr;
	}

	if (dsOutlineState)
	{
		dsOutlineState->Release();
		dsOutlineState = nullptr;
	}

	if (dsSkyState)
	{
		dsSkyState->Release();
		dsSkyState = nullptr;
	}

	if (dsGizmoState)
	{
		dsGizmoState->Release();
		dsGizmoState = nullptr;
	}

	if (dsState)
	{
		dsState->Release();
		dsState = nullptr;
	}

	if (depthStencilView)
	{
		depthStencilView->Release();
		depthStencilView = nullptr;
	}
}

void Renderer::SetDefaultDepthState()
{
	UINT stencilRef = 1;
	DeviceContext->OMSetDepthStencilState(dsState, stencilRef);
}

void Renderer::SetGizmoDepthState()
{
	UINT stencilRef = 1;
	DeviceContext->OMSetDepthStencilState(dsGizmoState, stencilRef);
}

void Renderer::SetSelectedState()
{
	UINT stencilRef = 1;
	DeviceContext->OMSetDepthStencilState(dsSelectedState, stencilRef);
}

void Renderer::SetOutlineState()
{
	UINT stencilRef = 1;
	DeviceContext->OMSetDepthStencilState(dsOutlineState, stencilRef);
}

void Renderer::SetSkyDepthState()
{
	UINT stencilRef = 1;
	DeviceContext->OMSetDepthStencilState(dsSkyState, stencilRef);
}

void Renderer::SetOutlineParams(float pixels)
{
	SetCustomColor(FLinearColor(pixels, ViewportInfo.Width, ViewportInfo.Height, 1.0f));
}

void Renderer::DrawOutline(AActor* targetActor)
{
	if (!targetActor || !targetActor->GetMesh()) return;

	Transform trans = targetActor->GetTransform();
	Mesh* mesh = targetActor->GetMesh();

	//셰이더 및 아웃라인 상태 설정
	PrepareOutlineShader(mesh->GetInputLayout());
	RENDERER.SetOutlineParams(5.0f);
	SetOutlineState();

	//메시보다 1.05배 큰 월드 행렬 구성
	FMatrix S = FMatrix::Scale(trans.Scale * 1.05f);
	FMatrix R = trans.Rotation.ToMatrix();
	FMatrix T = FMatrix::Translation(trans.Location);
	FMatrix outlineWorld = S * R * T;

	if (targetActor->worldBuffer)
	{
		targetActor->worldBuffer->SetMat(outlineWorld);
		targetActor->worldBuffer->SetVSBuffer(0);
	}

	//그리는 건 mesh에서만 진행
	mesh->SetColor(FLinearColor::Yellow);
	mesh->Render();

	DeviceContext->Draw(mesh->GetNumVertices(), 0);
	SetDefaultDepthState();
}

void Renderer::SwapBuffer()
{
	SwapChain->Present(1, 0);
	
}

void Renderer::Resize(UINT width, UINT height)
{
	if (!SwapChain || width == 0 || height == 0)
		return;

	// 기존 RenderTarget 연결 해제
	DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

	// 기존 화면 크기에 의존하는 리소스 제거
	ReleaseDepthStencil();
	ReleaseFrameBuffer();

	// SwapChain 자체 크기 변경
	SwapChain->ResizeBuffers(
		0,
		width,
		height,
		DXGI_FORMAT_UNKNOWN,
		0
	);

	// 새로운 크기로 다시 생성
	CreateFrameBuffer();
	CreateDepthStencil();
}