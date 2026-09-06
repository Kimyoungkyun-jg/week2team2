#pragma once

#include "AActor.h"
#include "Sphere.h"

class ACollider : public AActor
{
	DECLARE_CLASS(ACollider, AActor)

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
		return Mass * (transform.Scale.x * transform.Scale.x + transform.Scale.y * transform.Scale.y) / 12.0f;
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

class ACube : public ACollider
{
	DECLARE_CLASS(ACube, ACollider)

public:
	ACube()
	{
		InitVertexBuffer(cube_vertices);
	}
};

class ACircle : public ACollider
{
	DECLARE_CLASS(ACircle, ACollider)

public:
	ACircle()
	{
		Primitive = EPrimitive::Sphere;
		InitVertexBuffer(sphere_vertices);
	}
	float GetRadius() const { return transform.Scale.x * 0.5f; }
	virtual float GetInertia() const override
	{
		float r = GetRadius();
		return 0.5f * Mass * r * r;
	}
};
