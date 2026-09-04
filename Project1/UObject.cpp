#include "UObject.h"
#include "Global.h"
#include "CollisionManager.h"
#include "ObjectManager.h"
#include "TemplateLibrary.h"
#include "Renderer.h"



void UObject::Destroy()
{
	UObjectManager::GetInstance().Destroy(this);
}

void AActor::Draw(URenderer& renderer)
{
	if (Bitmap)
	{
		renderer.DrawWorldBitmap(Bitmap, Location, Rotation, Scale);
	}
	else
	{
		renderer.UpdateConstant(Location, Rotation, Scale);
		renderer.RenderPrimitive(Primitive);
	}
}

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
