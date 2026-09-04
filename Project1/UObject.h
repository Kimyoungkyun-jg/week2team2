#pragma once

#include "Renderer.h"
#include "enums.h"
#include "FVector.h"

class UObject
{
public:
	UObject()
	{
		++UIDMax;
		UID = UIDMax;
	}
	virtual ~UObject() {}
	int GetID() const { return UID; }


	virtual void Update(float deltatime);
	virtual void Render() {}

	virtual void Destroy();
	virtual void Tick(float deltaTime) {}
	
	void Active() { bIsActive = true; }
	void DeActive() { bIsActive = false; }
	bool const GetIsActive() { return bIsActive; }
private:
	inline static int UIDMax = 0;
	int UID = 0;
	bool bIsActive = true;
};

class AActor : public UObject
{
public:
	AActor() {}
	virtual ~AActor() {}
	virtual void Draw(Renderer& renderer);
	virtual void Render() override { Draw(Renderer::GetInstance()); }

	void SetLocation(const FVector& loc) { Location = loc; }
	void SetRotation(const float _Rotation) { Rotation = _Rotation; }
	void SetScale(const FVector& _Scale) { Scale = _Scale; }
	void SetPrimitive(EPrimitive _Primitive) { Primitive = _Primitive; }
	float GetRotation() const { return Rotation; }
	EPrimitive GetPrimitive() const { return Primitive; }
	FVector GetScale() const { return Scale; }
	FVector GetLocation() const { return Location; }

	virtual void Pressed(FVector _Location) {}
	virtual void Released(FVector _Location) {}


	void SetImage(const wchar_t* uri)
	{
		Bitmap = Renderer::GetInstance().LoadBitmapFromFile(uri);
	}

	void SetBitmap(ID2D1Bitmap* bmp)
	{
		Bitmap = bmp;
	}

	bool isInvalid = false;

protected:
	FVector Location = FVector(0, 0, 0);
	EPrimitive Primitive = EPrimitive::Rectangle;
	float Rotation = 0.0f;
	FVector Scale = { 1, 1, 1 };
	ID2D1Bitmap* Bitmap = nullptr;
};

class ACollider : public AActor
{
public:
	ACollider() {}
	virtual ~ACollider() {}

	virtual void Move(float deltaTime);

	FVector GetVelocity() const { return Velocity; }
	void SetVelocity(FVector _Vel) { Velocity = _Vel; }

	float GetMass() const { return Mass; }
	void SetMass(float _Mass) { Mass = _Mass; }

	float GetStaticFriction() const { return StaticFriction; }
	void SetStaticFriction(float _f) { StaticFriction = _f; }

	float GetDynamicFriction() const { return DynamicFriction; }
	void SetDynamicFriction(float _f) { DynamicFriction = _f; }

	float GetAngularVelocity() const { return AngularVelocity; }
	void SetAngularVelocity(float value) { AngularVelocity = value; }

	float GetRestitution() const { return Restitution; }
	void SetRestitution(float _r) { Restitution = _r; }

	virtual void PlaySFX() {}

	bool IsSleeping() const { return bSleeping; }
	void SetSleeping(bool value) { bSleeping = value; }
	float GetSleepTimer() const { return SleepTimer; }
	void SetSleepTimer(float value) { SleepTimer = value; }
	void WakeUp() { bSleeping = false; SleepTimer = 0.0f; }

	virtual void Pressed(FVector _Location) override;
	virtual void Released(FVector _Location) override;
	virtual float GetInertia() const
	{
		return Mass * (Scale.x * Scale.x + Scale.y * Scale.y) / 12.0f;
	}

	bool bEditing = false;
	bool bUseGravity = false;

protected:
	FVector Velocity;
	float StaticFriction = 0.5f;
	float DynamicFriction = 0.3f;
	float Mass = 10.0f;
	float AngularVelocity = 0.0f;
	float Restitution = 0.2f;
	float LinearDamping = 0.0f;
	float AngularDamping = 2.0f;
	bool bSleeping = false;
	float SleepTimer = 0.0f;
};

class ARectangle : public ACollider
{
public:
	ARectangle()
	{
		Primitive = EPrimitive::Rectangle;
	}
};

class ACircle : public ACollider
{
public:
	ACircle()
	{
		Primitive = EPrimitive::Circle;
	}
	float GetRadius() const { return Scale.x * 0.5f; }
	virtual float GetInertia() const override
	{
		float r = GetRadius();
		return 0.5f * Mass * r * r;
	}
};
