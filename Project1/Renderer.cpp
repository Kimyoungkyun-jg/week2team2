#include "pch.h"
#include "Renderer.h"
#include "Sphere.h"
#include "Camera.h"
#include "UObject.h"
#include "GlobalBuffer.h"
#include "AActor.h"
#include "FQuaternion.h"
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
}

void Renderer::Release()
{
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

void Renderer::SetCustomColor(const FLinearColor& color = { 0,0,0,0 })
{
	if (CustomColorBuffer)
	{
		CustomColorBuffer->SetColor(color);
		CustomColorBuffer->SetVSBuffer(2);
		CustomColorBuffer->SetPSBuffer(2);
	}
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

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
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
	rasterizerdesc.CullMode = D3D11_CULL_NONE;
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

	//Vertex & Pixel Shader 컴파일 및 생성
	ID3DBlob* vsBlob = nullptr;
	CreateVertexShader(shaderPath, "mainVS", &SimpleVertexShader, &vsBlob);
	CreatePixelShader(shaderPath, "mainPS", &SimplePixelShader);

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

	if (SimplePixelShader)
	{
		SimplePixelShader->Release();
		SimplePixelShader = nullptr;
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

	UINT stencilRef = 1; // 스텐실에 기록할 기준값
	DeviceContext->OMSetDepthStencilState(dsState, stencilRef);
}


void Renderer::UpdateFrameConstant()
{
	Camera& cam = Camera::GetInstance();
	cam.vpBuffer->SetMat(cam.GetViewMatrix() * cam.GetProjectionMatrix(wAspectRatio), cam.GetLocation());
}


void Renderer::Update()
{

}


void Renderer::CreateDepthStencil()
{
	//깊이 버퍼용 텍스쳐 생성
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

	//

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

	// 기즈모용 깊이 스텐실 상태 (깊이 테스트 비활성화로 항상 최상단 렌더링)
	D3D11_DEPTH_STENCIL_DESC gizmoDesc = {};
	gizmoDesc.DepthEnable = FALSE;
	gizmoDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	Device->CreateDepthStencilState(&gizmoDesc, &dsGizmoState);

	// 아웃라이너용 깊이 스텐실 상태 (1이 아니라면 아웃라이너 그리기)
	D3D11_DEPTH_STENCIL_DESC selectedDesc = {};

	selectedDesc.DepthEnable = TRUE;
	selectedDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	selectedDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	selectedDesc.StencilEnable = TRUE;
	selectedDesc.StencilReadMask = 0xFF;
	selectedDesc.StencilWriteMask = 0xFF;

	selectedDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	selectedDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;		// stencil 실패시
	selectedDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_REPLACE;	// stencil 통과, 깊이 실패
	selectedDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		// 둘다 통과시

	selectedDesc.BackFace = selectedDesc.FrontFace;

	Device->CreateDepthStencilState(&selectedDesc, &dsSelectedState);


	// 아웃라이너용 깊이 스텐실 상태 (1이 아니라면 아웃라이너 그리기)
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

void Renderer::ReleaseDepthStencil()
{
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

void Renderer::DrawOutline(AActor* targetActor)
{
	if (!targetActor || !targetActor->GetMesh()) return;

	Mesh* mesh = targetActor->GetMesh();
	const Transform& transform = targetActor->GetTransform();

	// 셰이더 및 아웃라인 상태 설정
	PrepareShader(mesh->GetInputLayout());
	SetCustomColor(FLinearColor::Yellow);
	SetOutlineState();

	// 메시보다 1.05배 큰 월드 행렬 구성
	FMatrix S = FMatrix::Scale(transform.Scale * 1.05f);
	FMatrix R = transform.Rotation.ToMatrix();
	FMatrix T = FMatrix::Translation(transform.Location);
	FMatrix outlineWorld = S * R * T;

	if (targetActor->worldBuffer)
	{
		targetActor->worldBuffer->SetMat(outlineWorld);
		targetActor->worldBuffer->SetVSBuffer(0);
	}

	//그리는 건 mesh에서만 진행
	mesh->SetColor(FLinearColor::Yellow);
	mesh->Render();

	//원래 월드 행렬 및 기본 깊이 복원
	targetActor->SetWorldBuffer();
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