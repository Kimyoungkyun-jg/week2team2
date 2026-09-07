#pragma once
#include <d3d11.h>

struct FVertexSimple
{
	float x, y, z;    // Position
};

struct FVertexColor
{
	float x, y, z;    // Position
	float r, g, b, a; // Color
};

struct FVertexUV
{
	float x, y, z;    // Position
	float u, v;       // UV
};

// 정점 구조체별 Direct3D 11 Input Layout 메타데이터 템플릿
template<typename T>
struct FVertexLayouts;

template<>
struct FVertexLayouts<FVertexSimple>
{
	static inline const D3D11_INPUT_ELEMENT_DESC Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	static constexpr UINT NumElements = 1;
};

template<>
struct FVertexLayouts<FVertexColor>
{
	static inline const D3D11_INPUT_ELEMENT_DESC Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	static constexpr UINT NumElements = 2;
};

template<>
struct FVertexLayouts<FVertexUV>
{
	static inline const D3D11_INPUT_ELEMENT_DESC Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	static constexpr UINT NumElements = 2;
};