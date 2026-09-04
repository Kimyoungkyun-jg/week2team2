#pragma once

#include "UObject.h"
#include "enums.h"
#include "FVector.h"

using namespace DirectX;


class AActor : public UObject
{
public:
	AActor();
	virtual ~AActor() {}
	virtual void Render() override;
	virtual void Update(float Deltatime) override;

	void SetLocation(const FVector& loc) { Location = loc; }
	void SetRotation(const FVector& _Rotation) { Rotation = _Rotation; }
	void SetScale(const FVector& _Scale) { Scale = _Scale; }

	void SetPrimitive(EPrimitive _Primitive) { Primitive = _Primitive; }
	FVector GetRotation() const { return Rotation; }
	EPrimitive GetPrimitive() const { return Primitive; }
	FVector GetScale() const { return Scale; }
	FVector GetLocation() const { return Location; }

	virtual void Pressed(FVector _Location) {}
	virtual void Released(FVector _Location) {}

	bool isInvalid = false;

protected:
	FVector Location = FVector(0, 0, 0);
	EPrimitive Primitive = EPrimitive::Cube;
	FVector Rotation = FVector(0, 0, 0);
	FVector Scale = { 1, 1, 1 };
	ID2D1Bitmap* Bitmap = nullptr;


	DirectX::XMMATRIX worldmat;

};
