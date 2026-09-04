#include "pch.h"
#include "Renderer.h"
#include "Sphere.h"	

void Renderer::Create(HWND hWindow)
{
	CreateDeviceAndSwapChain(hWindow);
	CreateFrameBuffer();
	CreateRasterizerState();

}

void Renderer::Release()
{
	ReleaseRasterizerState();
	if (DeviceContext)
	{
		DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	}
	ReleaseFrameBuffer();
	ReleaseDeviceAndSwapChain();
}

void Renderer::CreateDeviceAndSwapChain(HWND hWindow)
{
	D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

	DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
	swapchaindesc.BufferDesc.Width = 0;
	swapchaindesc.BufferDesc.Height = 0;
	swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapchaindesc.SampleDesc.Count = 1;
	swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchaindesc.BufferCount = 2;
	swapchaindesc.OutputWindow = hWindow;
	swapchaindesc.Windowed = TRUE;
	swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
		featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION,
		&swapchaindesc, &SwapChain, &Device, nullptr, &DeviceContext);

	SwapChain->GetDesc(&swapchaindesc);
	ViewportInfo = { 0.0f, 0.0f,
		(float)swapchaindesc.BufferDesc.Width, (float)swapchaindesc.BufferDesc.Height,
		0.0f, 1.0f };

	wAspectRatio = (float)swapchaindesc.BufferDesc.Width / (float)swapchaindesc.BufferDesc.Height;
}

void Renderer::ReleaseDeviceAndSwapChain()
{
	if (DeviceContext)
	{
		DeviceContext->Flush();
	}

	if (SwapChain)
	{
		SwapChain->Release();
		SwapChain = nullptr;
	}

	if (Device)
	{
		Device->Release();
		Device = nullptr;
	}

	if (DeviceContext)
	{
		DeviceContext->Release();
		DeviceContext = nullptr;
	}
}

void Renderer::CreateFrameBuffer()
{
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&FrameBuffer);

	Device->CreateRenderTargetView(FrameBuffer, nullptr, &FrameBufferRTV);
}

void Renderer::ReleaseFrameBuffer()
{
	if (FrameBuffer)
	{
		FrameBuffer->Release();
		FrameBuffer = nullptr;
	}

	if (FrameBufferRTV)
	{
		FrameBufferRTV->Release();
		FrameBufferRTV = nullptr;
	}
}

void Renderer::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterizerdesc = {};
	rasterizerdesc.FillMode = D3D11_FILL_SOLID;
	rasterizerdesc.CullMode = D3D11_CULL_NONE;

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

void Renderer::CreateShader()
{
	ID3DBlob* vertexshaderCSO = nullptr;
	ID3DBlob* pixelshaderCSO = nullptr;
	ID3DBlob* errorBlob = nullptr;

	const wchar_t* candidatePaths[] = {
		L"ShaderW0.hlsl",
		L"Project1/ShaderW0.hlsl",
		L"../Project1/ShaderW0.hlsl",
		L"bin/Debug/ShaderW0.hlsl",
		L"../bin/Debug/ShaderW0.hlsl"
	};

	const wchar_t* shaderPath = L"ShaderW0.hlsl";
	for (const wchar_t* path : candidatePaths)
	{
		if (std::filesystem::exists(path))
		{
			shaderPath = path;
			break;
		}
	}

	// Vertex Shader 컴파일
	HRESULT hr = D3DCompileFromFile(
		shaderPath, nullptr, nullptr,
		"mainVS", "vs_5_0", 0, 0, &vertexshaderCSO, &errorBlob);

	if (FAILED(hr) || !vertexshaderCSO)
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		OutputDebugStringA("Failed to compile vertex shader!\n");
		return;
	}

	Device->CreateVertexShader(
		vertexshaderCSO->GetBufferPointer(),
		vertexshaderCSO->GetBufferSize(), nullptr, &SimpleVertexShader);

	// Pixel Shader 컴파일
	hr = D3DCompileFromFile(
		shaderPath, nullptr, nullptr,
		"mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, &errorBlob);

	if (FAILED(hr) || !pixelshaderCSO)
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		vertexshaderCSO->Release();
		return;
	}

	Device->CreatePixelShader(
		pixelshaderCSO->GetBufferPointer(),
		pixelshaderCSO->GetBufferSize(), nullptr, &SimplePixelShader);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	Device->CreateInputLayout(
		layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(),
		vertexshaderCSO->GetBufferSize(), &SimpleInputLayout);

	Stride = sizeof(FVertexSimple);

	vertexshaderCSO->Release();
	pixelshaderCSO->Release();
}

void Renderer::ReleaseShader()
{
	if (SimpleInputLayout)
	{
		SimpleInputLayout->Release();
		SimpleInputLayout = nullptr;
	}

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

void Renderer::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC constantbufferdesc = {};
	constantbufferdesc.ByteWidth = sizeof(FConstants) + 0xf & 0xfffffff0;
	constantbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	constantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	Device->CreateBuffer(&constantbufferdesc, nullptr, &ConstantBuffer);

	constants.worldmat = XMMatrixIdentity();
	constants.viewmat = XMMatrixIdentity();
	constants.projmat = XMMatrixTranspose(XMMatrixScaling(1.0f / wAspectRatio, 1.0f, 1.0f));
	constants.AspectRatio = 0;
}

void Renderer::ReleaseConstantBuffer()
{
	if (ConstantBuffer)
	{
		ConstantBuffer->Release();
		ConstantBuffer = nullptr;
	}
}

void Renderer::CreateVertexBufferInfos()
{
	UINT numVerticesCube = sizeof(cube_vertices) / sizeof(FVertexSimple);
	UINT numVerticesSphere = sizeof(sphere_vertices) / sizeof(FVertexSimple);

	ID3D11Buffer* vertexBufferCube = CreateVertexBuffer(cube_vertices, sizeof(cube_vertices));
	ID3D11Buffer* vertexBufferSphere = CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));

	VertexBufferInfos.push_back({ vertexBufferSphere , numVerticesSphere });
	VertexBufferInfos.push_back({ vertexBufferCube , numVerticesCube });
}


ID3D11Buffer* Renderer::CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth)
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

void Renderer::ReleaseVertexBuffers()
{
	for (FVertexBufferInfo& vi : VertexBufferInfos)
	{
		ReleaseVertexBuffer(vi.vertexBuffer);
		vi.vertexBuffer = nullptr;
	}
	VertexBufferInfos.clear();
}

void Renderer::ReleaseVertexBuffer(ID3D11Buffer* vertexBuffer)
{
	if (vertexBuffer)
	{
		vertexBuffer->Release();
	}
}

void Renderer::Prepare()
{
	DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);

	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DeviceContext->RSSetViewports(1, &ViewportInfo);
	DeviceContext->RSSetState(RasterizerState);

	DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, nullptr);
	DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
}

void Renderer::PrepareShader()
{
	DeviceContext->IASetInputLayout(SimpleInputLayout);

	DeviceContext->VSSetShader(SimpleVertexShader, nullptr, 0);
	if (ConstantBuffer)
	{
		DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
	}

	DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
}


void Renderer::Update()
{
	if (ConstantBuffer)
	{
		D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

		HRESULT hr = DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR);
		if (SUCCEEDED(hr))
		{
			memcpy(constantbufferMSR.pData, &constants, sizeof(FConstants));
			DeviceContext->Unmap(ConstantBuffer, 0);
		}
	}
}

void Renderer::SetWorldMatrix(XMMATRIX worldmat)
{
	constants.worldmat = XMMatrixTranspose(worldmat);
}

void Renderer::SetViewMatrix(XMMATRIX viewmat)
{
	constants.viewmat = XMMatrixTranspose(viewmat);
}

void Renderer::SetProjMatrix(XMMATRIX projmat)
{
	constants.projmat = XMMatrixTranspose(projmat);
}

void Renderer::SetVSBuffer(UINT slot)
{
	Update();
	DeviceContext->VSSetConstantBuffers(slot, 1, &ConstantBuffer);
}

void Renderer::RenderPrimitive(EPrimitive Primitive)
{
	UINT offset = 0;

	ID3D11Buffer* pBuffer = nullptr;
	UINT numVertices;
	if (Primitive == EPrimitive::Circle)
	{ 
		pBuffer = VertexBufferInfos[0].vertexBuffer;
		numVertices = VertexBufferInfos[0].numVertucies;
	}
	else if (Primitive == EPrimitive::Cube)
	{
		pBuffer = VertexBufferInfos[1].vertexBuffer;
		numVertices = VertexBufferInfos[1].numVertucies;
	}

	DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);
	DeviceContext->Draw(numVertices, 0);
}

void Renderer::SwapBuffer()
{
	SwapChain->Present(1, 0);
}

