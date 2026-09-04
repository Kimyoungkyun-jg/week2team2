#include "pch.h"
#include "D3D11Util.h"
#include "Renderer.h"

void D3D11Util::CreateVertexShaderAndInputLayout(
    const LPCWSTR& fileName,
    ID3D11Device* device,
    ID3D11VertexShader** vertexShader,
    ID3D11InputLayout** inputLayout)
{
    ID3DBlob* vertexshaderCSO = nullptr;
    ID3DBlob* errorBlob = nullptr;

    HRESULT hr = D3DCompileFromFile(fileName, nullptr, nullptr,
        "mainVS", "vs_5_0", 0, 0, &vertexshaderCSO, &errorBlob);

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return;
    }

    device->CreateVertexShader(
        vertexshaderCSO->GetBufferPointer(),
        vertexshaderCSO->GetBufferSize(),
        nullptr,
        vertexShader);         

    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    device->CreateInputLayout(inputElementDesc,
        ARRAYSIZE(inputElementDesc),
        vertexshaderCSO->GetBufferPointer(),
        vertexshaderCSO->GetBufferSize(),
        inputLayout);          

    vertexshaderCSO->Release();
}

void D3D11Util::CreatePixelShader(const LPCWSTR& fileName, ID3D11PixelShader** pixelShader)
{
    ID3DBlob* pixelshaderCSO = nullptr;

    HRESULT hr = D3DCompileFromFile(fileName, nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);

	if (pixelshaderCSO)
	{
		Renderer::GetInstance().Device->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, pixelShader);
		pixelshaderCSO->Release();
	}
}

void D3D11Util::CreateSamplerState(ID3D11SamplerState** samplerState)
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	HRESULT hr = Renderer::GetInstance().Device->CreateSamplerState(&samplerDesc, samplerState);
}

void D3D11Util::CreateTexture(const LPCWSTR& fileName, ID3D11Texture2D** texture, ID3D11ShaderResourceView** srv)
{
	DirectX::ScratchImage scratchImage;

	HRESULT hr = DirectX::LoadFromWICFile(
		fileName,
		DirectX::WIC_FLAGS_NONE,
		nullptr,
		scratchImage
	);

	if (FAILED(hr))
	{
		OutputDebugStringA("Failed to load texture image\n");
		return;
	}

	const DirectX::Image* image = scratchImage.GetImage(0, 0, 0);

	if (image == nullptr)
	{
		OutputDebugStringA("Failed to get image data\n");
		return;
	}

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = static_cast<UINT>(image->width);
	textureDesc.Height = static_cast<UINT>(image->height);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = image->format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = image->pixels;
	initData.SysMemPitch = static_cast<UINT>(image->rowPitch);
	initData.SysMemSlicePitch = static_cast<UINT>(image->slicePitch);

	hr = Renderer::GetInstance().Device->CreateTexture2D(
		&textureDesc,
		&initData,
		texture
	);

	if (FAILED(hr))
	{
		OutputDebugStringA("Failed to create Texture2D\n");
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = Renderer::GetInstance().Device->CreateShaderResourceView(
		*texture,
		&srvDesc,
		srv
	);

	if (FAILED(hr))
	{
		OutputDebugStringA("Failed to create SRV\n");
		(*texture)->Release();   // 이미 만들어진 텍스처 정리
		*texture = nullptr;
		return;
	}
}


