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
	void SetRotation(const FQuaternion& _Rotation) { transform.SetRotation(_Rotation); }
	void SetScale(const FVector& _Scale) { transform.SetScale(_Scale); }

	void SetPrimitive(EPrimitive _Primitive) { Primitive = _Primitive; }
	EPrimitive GetPrimitive() const { return Primitive; }
	const FQuaternion GetRotation() const { return transform.GetRotation(); }
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

	virtual bool bIsPicked(const FRay& worldRay, float& outDistance);
	virtual bool bIsPicked(const FRay& worldRay)
	{
		float dummyDist = 0.0f;
		return bIsPicked(worldRay, dummyDist);
	}
	bool IsSelected() const;

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
};


