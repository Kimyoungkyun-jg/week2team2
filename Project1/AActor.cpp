#include "pch.h"
#include "AActor.h"
#include "Renderer.h"
#include "PickingManager.h"
#include "Intersection.h"
#include "AGizmo.h"

AActor::AActor(const FLinearColor& inColor)
	: Color(inColor)
{
	worldBuffer = new MatrixBuffer();
}

AActor::~AActor()
{
	if (worldBuffer)
	{
		delete worldBuffer;
		worldBuffer = nullptr;
	}

	if (bOwnsMesh && mesh)
	{
		delete mesh;
		mesh = nullptr;
	}
}

bool AActor::bIsPicked(const FRay& worldRay, float& outDistance)
{
	if (mesh)
	{
		return mesh->bIsPicked(worldRay, transform, outDistance);
	}
	return false;
}

void AActor::SetWorldBuffer()
{
	worldBuffer->SetMat(transform.WorldMat);
	worldBuffer->SetVSBuffer(0);
}

bool AActor::IsSelected() const
{
	return AGizmo::MainGizmo && this == AGizmo::MainGizmo->GetTargetActor();
}

void AActor::Render()
{
	UObject::Render();

	SetWorldBuffer();

	if (mesh)
	{
		const bool bSelected = IsSelected();
		if (bSelected)
		{
			RENDERER.SetSelectedState();
		}

		mesh->SetColor(Color);
		mesh->Render();

		if (bSelected)
		{
			RENDERER.SetDefaultDepthState();
		}
	}
}

void AActor::Update(float Deltatime)
{
	UObject::Update(Deltatime);
}

