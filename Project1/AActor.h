#pragma once

#include "FVertexSimple.h"
#include "UObject.h"
#include "enums.h"
#include "Transform.h"
#include "VertexBuffer.h"
#include "GlobalBuffer.h"

using namespace DirectX;


class AActor : public UObject
{
	DECLARE_CLASS(AActor, UObject)

public:
	AActor();
	virtual ~AActor();
	virtual void Render() override;
	virtual void Update(float Deltatime) override;


	void SetLocation(const FVector& loc) { transform.SetLocation(loc); }
	void SetRotation(const FVector& _Rotation) { transform.SetRotation(_Rotation); }
	void SetScale(const FVector& _Scale) { transform.SetScale(_Scale); }

	void SetPrimitive(EPrimitive _Primitive) { Primitive = _Primitive; }
	const FVector& GetRotation() const { return transform.GetRotation(); }
	EPrimitive GetPrimitive() const { return Primitive; }
	const FVector& GetScale() const { return transform.GetScale(); }
	const FVector& GetLocation() const { return transform.GetLocation(); }

	Transform& GetTransform() { return transform; }
	const Transform& GetTransform() const { return transform; }
	void SetTransform(const Transform& inTransform) { transform = inTransform; }

	// 커스텀 정점 버퍼 초기화 함수
	void InitVertexBuffer(const void* vertices, UINT stride, UINT inNumVertices, ID3D11InputLayout* inLayout = nullptr);

	// 정점 배열을 넘기면 타입(VertexType), 정점 개수, InputLayout까지 자동 추론 및 저장!
	template <typename VertexType, size_t N>
	void InitVertexBuffer(const VertexType (&vertices)[N])
	{
		InitVertexBuffer(vertices, sizeof(VertexType), static_cast<UINT>(N), REDERER.GetInputLayout<VertexType>());
	}

	UINT GetNumVertices() const { return numVertices; }

	virtual void Pressed(FVector _Location) {}
	virtual void Released(FVector _Location) {}

	bool isInvalid = false;

public:
	Transform transform;
	EPrimitive Primitive = EPrimitive::Cube;
	
	MatrixBuffer* worldBuffer = nullptr;
	VertexBuffer* vertexbuffer = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;
	UINT numVertices = 0;
};

