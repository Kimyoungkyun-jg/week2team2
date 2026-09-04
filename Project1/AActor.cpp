#include "pch.h"
#include "AActor.h"
#include "Renderer.h"


AActor::AActor()
{
	worldmat = XMMatrixIdentity();
}

void AActor::Render()
{
	UObject::Render();

	Renderer::GetInstance().SetWorldMatrix(worldmat);
	Renderer::GetInstance().SetVSBuffer(0);
	Renderer::GetInstance().RenderPrimitive(Primitive);
}

void AActor::Update(float Deltatime)
{
	UObject::Update(Deltatime);

	XMMATRIX S = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	XMMATRIX R = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
	XMMATRIX T = XMMatrixTranslation(Location.x, Location.y, Location.z);

	worldmat = S * R * T;

	Renderer::GetInstance().Update();
}

