#pragma once

#include "FVertexSimple.h"
#include "UObject.h"
#include "enums.h"
#include "Transform.h"
#include "VertexBuffer.h"
#include "GlobalBuffer.h"
#include "FLinearColor.h"
#include "Mesh.h"

using namespace DirectX;


struct FRay;

class AActor : public UObject
{
	DECLARE_CLASS(AActor, UObject)

public:
	AActor(const FLinearColor& inColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));
	virtual ~AActor();
	virtual void Render() override;
	virtual void Update(float Deltatime) override;

	void SetLocation(const FVector& loc) { transform.SetLocation(loc); }
	void SetRotation(const FVector& _Rotation) { transform.SetRotation(_Rotation); }
	void SetScale(const FVector& _Scale) { transform.SetScale(_Scale); }

	void SetPrimitive(EPrimitive _Primitive) { Primitive = _Primitive; }
	EPrimitive GetPrimitive() const { return Primitive; }
	const FVector& GetRotation() const { return transform.GetRotation(); }
	const FVector& GetScale() const { return transform.GetScale(); }
	const FVector& GetLocation() const { return transform.GetLocation(); }

	Transform& GetTransform() { return transform; }
	const Transform& GetTransform() const { return transform; }
	void SetTransform(const Transform& inTransform) { transform = inTransform; }

	void SetColor(const FLinearColor& inColor) { Color = inColor; }
	const FLinearColor& GetColor() const { return Color; }

	void SetMesh(Mesh* inMesh, bool bOwned = false)
	{
		if (bOwnsMesh && mesh && mesh != inMesh)
		{
			delete mesh;
		}
		mesh = inMesh;
		bOwnsMesh = bOwned;
	}
	Mesh* GetMesh() const { return mesh; }

	// 커스텀 정점 버퍼 초기화 함수 (독자 소유 메시 생성)
	void InitVertexBuffer(const void* vertices, UINT stride, UINT inNumVertices, ID3D11InputLayout* inLayout = nullptr)
	{
		if (bOwnsMesh && mesh)
		{
			delete mesh;
			mesh = nullptr;
		}
		mesh = new Mesh();
		bOwnsMesh = true;
		mesh->InitVertexBuffer(vertices, stride, inNumVertices, inLayout);
	}

	// 정점 배열을 넘기면 타입(VertexType), 정점 개수, InputLayout까지 자동 추론 및 저장!
	template <typename VertexType, size_t N>
	void InitVertexBuffer(const VertexType(&vertices)[N])
	{
		if (bOwnsMesh && mesh)
		{
			delete mesh;
			mesh = nullptr;
		}
		mesh = new Mesh();
		bOwnsMesh = true;
		mesh->InitVertexBuffer(vertices);
	}

	// std::vector 정점 배열 초기화
	template <typename VertexType>
	void InitVertexBuffer(const std::vector<VertexType>& vertices)
	{
		if (bOwnsMesh && mesh)
		{
			delete mesh;
			mesh = nullptr;
		}
		mesh = new Mesh();
		bOwnsMesh = true;
		mesh->InitVertexBuffer(vertices);
	}

	// TArray 정점 배열 초기화
	template <typename VertexType>
	void InitVertexBuffer(const TArray<VertexType>& vertices)
	{
		if (bOwnsMesh && mesh)
		{
			delete mesh;
			mesh = nullptr;
		}
		mesh = new Mesh();
		bOwnsMesh = true;
		mesh->InitVertexBuffer(vertices);
	}

	virtual bool bIsPicked(const FRay& worldRay, float& outDistance);
	virtual bool bIsPicked(const FRay& worldRay)
	{
		float dummyDist = 0.0f;
		return bIsPicked(worldRay, dummyDist);
	}
	void SetSelected(bool inSelected) { bIsSelected = inSelected; }
	bool IsSelected() const { return bIsSelected; }

	virtual void Pressed() {}
	virtual void Released() {}

	void SetWorldBuffer();

	bool isInvalid = false;

public:
	Transform transform;
	EPrimitive Primitive = EPrimitive::Cube;
	MatrixBuffer* worldBuffer = nullptr;
	FLinearColor Color = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	Mesh* mesh = nullptr;
	bool bOwnsMesh = false;
	bool bIsSelected = false;
};


