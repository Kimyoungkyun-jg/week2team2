#include "pch.h"
#include "Renderer.h"
#include "Sphere.h"
#include "Camera.h"
#include "UObject.h"
#include "GlobalBuffer.h"
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

void Renderer::SetCustomColor(const FLinearColor& color)
{
	if (CustomColorBuffer)
	{
		CustomColorBuffer->SetColor(color);
		CustomColorBuffer->SetVSBuffer(2);
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
	Camera::GetInstance().vpBuffer->SetMat(Camera::GetInstance().GetViewMatrix() * Camera::GetInstance().GetProjectionMatrix(wAspectRatio));
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
}

void Renderer::ReleaseDepthStencil()
{
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

void Renderer::SwapBuffer()
{
	SwapChain->Present(1, 0);
}

