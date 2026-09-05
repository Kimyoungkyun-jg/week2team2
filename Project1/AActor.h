#pragma once

#include "UObject.h"
#include "enums.h"
#include "Transform.h"
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

	virtual void Pressed(FVector _Location) {}
	virtual void Released(FVector _Location) {}

	bool isInvalid = false;

public:
	Transform transform;
	EPrimitive Primitive = EPrimitive::Cube;
	

	MatrixBuffer* worldBuffer;

};

