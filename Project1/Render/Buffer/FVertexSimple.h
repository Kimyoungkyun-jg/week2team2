#pragma once
#include <d3d11.h>

// 공용 정점 구조체 (48byte)
struct FVertexData
{
	float x = 0.0f, y = 0.0f, z = 0.0f;          // Position
	float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f; // Color
	float u = 0.0f, v = 0.0f;                      // UV
	float nx = 0.0f, ny = 0.0f, nz = 0.0f;         // Normal
};

// 공용 Direct3D 11 Input Layout 메타데이터
struct FVertexLayouts
{
	static inline const D3D11_INPUT_ELEMENT_DESC Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	static constexpr UINT NumElements = 4;
};