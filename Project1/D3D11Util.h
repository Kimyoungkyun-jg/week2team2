#pragma once

#include "App.h"
#include "FVertexSimple.h"

class D3D11Util
{
public:
	static void CreateVertexShaderAndInputLayout(
		const LPCWSTR& fileName,
		ID3D11Device* device,
		ID3D11VertexShader** vertexShader,
		ID3D11InputLayout** inputLayout);

	static void CreatePixelShader(const LPCWSTR& fileName, ID3D11PixelShader** pixelShader);

	static void CreateSamplerState(ID3D11SamplerState** samplerState);

	

	template<typename T>
	static void UpdateConstantBuffer(ID3D11DeviceContext* deviceContext,
		ID3D11Buffer* constantBuffer,
		const T& data)
	{
		D3D11_MAPPED_SUBRESOURCE constantbufferMSR;
		deviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR);
		memcpy(constantbufferMSR.pData, &data, sizeof(T));

		deviceContext->Unmap(constantBuffer, 0);
	};

	static void CreateTexture(const LPCWSTR& fileName, ID3D11Texture2D** texture, ID3D11ShaderResourceView** srv);




};