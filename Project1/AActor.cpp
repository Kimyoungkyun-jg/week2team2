#include "pch.h"
#include "AActor.h"
#include "Renderer.h"


AActor::AActor()
{
	worldBuffer = new MatrixBuffer();
}

AActor::~AActor()
{
	delete worldBuffer;
}

void AActor::Render()
{
	UObject::Render();

	worldBuffer->SetMat(transform.WorldMat);
	worldBuffer->SetVSBuffer(0);

	REDERER.RenderPrimitive(Primitive);
}

void AActor::Update(float Deltatime)
{
	UObject::Update(Deltatime);
}

