#include "pch.h"
#include "ACollider.h"
#include "Global.h"

void ACollider::Move(float deltaTime)
{
	if (Mass <= 0.0f || bSleeping)
	{
		return;
	}

	if (bUseGravity)
	{
		Velocity += Global::G * deltaTime;
	}

	Velocity = Velocity * (1.0f / (1.0f + deltaTime * LinearDamping));
	AngularVelocity *= 1.0f / (1.0f + deltaTime * AngularDamping);

	Location += Velocity * deltaTime;
	Rotation += AngularVelocity * deltaTime;
}

void ACollider::Pressed(FVector _Location)
{
	if (bEditing)
	{
		WakeUp();
		Location = _Location;
		Velocity = FVector();
		bUseGravity = false;
	}
}

void ACollider::Released(FVector _Location)
{
	if (bEditing)
	{
		WakeUp();
		Location = _Location;
		Velocity = FVector();
		bUseGravity = true;
	}
}
