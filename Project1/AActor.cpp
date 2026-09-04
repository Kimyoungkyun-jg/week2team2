#include "pch.h"
#include "AActor.h"
#include "Renderer.h"


AActor::AActor()
{
}

void AActor::Render()
{
	UObject::Render();

	Renderer::GetInstance().SetWorldMatrix(transform.GetWorldMatrix());
	Renderer::GetInstance().SetVSBuffer(0);
	Renderer::GetInstance().RenderPrimitive(Primitive);
}

void AActor::Update(float Deltatime)
{
	UObject::Update(Deltatime);

	transform.UpdateWorldMatrix();


	Renderer::GetInstance().Update();
}

