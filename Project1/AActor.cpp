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

void AActor::DrawWithSelection(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	vertexbuffer->IASet(topology);

	const bool bSelected = IsSelected();
	if (bSelected) {
		RENDERER.SetSelectedState();
	}

	RENDERER.GetDeviceContext()->Draw(numVertices, 0);

	if (bSelected)
		RENDERER.SetDefaultDepthState();
}

void AActor::Render()
{
	UObject::Render();

	SetWorldBuffer();

	if (mesh)
	{
		mesh->SetColor(Color);
		mesh->Render();
	}
}

void AActor::Update(float Deltatime)
{
	UObject::Update(Deltatime);
}

