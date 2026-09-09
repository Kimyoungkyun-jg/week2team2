#pragma once
#include "VertexBuffer.h"
#include "Renderer.h"
#include "Containers.h"
#include "Transform.h"
#include "FLinearColor.h"

struct FRay;

class Mesh
{
public:
	Mesh();
	~Mesh();

	// 정점 배열을 받아 바로 초기화하는 생성자
	template <typename VertexType, size_t N>
	Mesh(const VertexType(&vertices)[N])
	{
		InitVertexBuffer(vertices);
	}

	// 정점 벡터를 받아 바로 초기화하는 생성자
	template <typename VertexType>
	Mesh(const std::vector<VertexType>& vertices)
	{
		InitVertexBuffer(vertices);
	}

	// 정점 TArray를 받아 바로 초기화하는 생성자
	template <typename VertexType>
	Mesh(const TArray<VertexType>& vertices)
	{
		InitVertexBuffer(vertices);
	}

	// 커스텀 정점 버퍼 초기화 함수
	void InitVertexBuffer(const void* vertices, UINT stride, UINT inNumVertices, ID3D11InputLayout* inLayout = nullptr);

	// 정점 배열을 넘기면 타입(VertexType), 정점 개수, InputLayout까지 자동 추론 및 저장!
	template <typename VertexType, size_t N>
	void InitVertexBuffer(const VertexType(&vertices)[N])
	{
		InitVertexBuffer(vertices, sizeof(VertexType), static_cast<UINT>(N), RENDERER.GetInputLayout<VertexType>());

		LocalVertices.clear();
		LocalVertices.reserve(N);

		for (size_t i = 0; i < N; ++i)
		{
			LocalVertices.push_back(FVector(vertices[i].x, vertices[i].y, vertices[i].z));
		}
	}

	// std::vector 정점 배열 초기화
	template <typename VertexType>
	void InitVertexBuffer(const std::vector<VertexType>& vertices)
	{
		if (vertices.empty()) return;

		InitVertexBuffer(vertices.data(), sizeof(VertexType), static_cast<UINT>(vertices.size()), RENDERER.GetInputLayout<VertexType>());

		LocalVertices.clear();
		LocalVertices.reserve(vertices.size());

		for (size_t i = 0; i < vertices.size(); ++i)
		{
			LocalVertices.push_back(FVector(vertices[i].x, vertices[i].y, vertices[i].z));
		}
	}

	// TArray 정점 배열 초기화
	template <typename VertexType>
	void InitVertexBuffer(const TArray<VertexType>& vertices)
	{
		if (vertices.empty()) return;

		InitVertexBuffer(vertices.data(), sizeof(VertexType), static_cast<UINT>(vertices.size()), RENDERER.GetInputLayout<VertexType>());

		LocalVertices.clear();
		LocalVertices.reserve(vertices.size());

		for (size_t i = 0; i < vertices.size(); ++i)
		{
			LocalVertices.push_back(FVector(vertices[i].x, vertices[i].y, vertices[i].z));
		}
	}

	void SetColor(const FLinearColor& inColor) { CurrentColor = inColor; }
	const FLinearColor& GetColor() const { return CurrentColor; }

	// 텍스처 설정 및 조회
	void SetTexture(ID3D11ShaderResourceView* inSRV) { TextureSRV = inSRV; }
	void SetTexture(const std::wstring& path) { TextureSRV = RENDERER.LoadTexture(path); }
	ID3D11ShaderResourceView* GetTexture() const { return TextureSRV; }

	void Render();
	void Render(const FLinearColor& color);
	void Render(D3D11_PRIMITIVE_TOPOLOGY topology);
	void Render(const FLinearColor& color, D3D11_PRIMITIVE_TOPOLOGY topology);
	void IASet(D3D11_PRIMITIVE_TOPOLOGY type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	bool bIsPicked(const FRay& worldRay, const Transform& transform, float& outDistance);

	UINT GetNumVertices() const { return numVertices; }
	ID3D11InputLayout* GetInputLayout() const { return inputLayout; }
	VertexBuffer* GetVertexBuffer() const { return vertexbuffer; }

public:
	VertexBuffer* vertexbuffer = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;
	ID3D11ShaderResourceView* TextureSRV = nullptr;
	UINT numVertices = 0;
	FLinearColor CurrentColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	TArray<FVector> LocalVertices; // 마우스 피킹용 로컬 정점 데이터
};


