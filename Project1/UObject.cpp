#include "pch.h"
#include "UObject.h"
#include "Global.h"
#include "CollisionManager.h"
#include "ObjectManager.h"
#include "TemplateLibrary.h"
#include "Renderer.h"
#include "Camera.h"



void UObject::Destroy()
{
	ObjectManager::GetInstance().Destroy(this);
}

void UObject::Update(float deltatime)
{
	if (bIsActive) return;
}

void AActor::Draw(Renderer& renderer)
{
	if (Bitmap)
	{
		renderer.DrawWorldBitmap(Bitmap, Location, Rotation, Scale);
	}
	else
	{
		Camera& camera = Camera::GetInstance();
		DirectX::XMMATRIX world = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z) * DirectX::XMMatrixRotationZ(Rotation) * DirectX::XMMatrixTranslation(Location.x, Location.y, Location.z);
		DirectX::XMMATRIX view = camera.GetViewMatrix();
		DirectX::XMMATRIX projection = camera.GetProjectionMatrix(Renderer::GetInstance().GetAspectRatio());
		
		DirectX::XMMATRIX WVP = world * view * projection;
		
		
		renderer.UpdateConstant(Location, Rotation, Scale, WVP);
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
