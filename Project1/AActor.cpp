#include "pch.h"
#include "AActor.h"
#include "Renderer.h"
#include "PickingManager.h"
#include "Intersection.h"

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
	if (!mesh) return;

	mesh->GetVertexBuffer()->IASet(topology);

	const bool bSelected = IsSelected();
	if (bSelected) {
		RENDERER.SetSelectedState();
	}

	mesh->SetColor(Color);
	mesh->Render(topology);

	if (bSelected)
		RENDERER.SetDefaultDepthState();
}

void AActor::Render()
{
	UObject::Render();

	SetWorldBuffer();

	DrawWithSelection();
}

void AActor::Update(float Deltatime)
{
	UObject::Update(Deltatime);
}

void AActor::RenderOutline()
{
	RENDERER.PrepareShader(mesh->GetInputLayout());
	RENDERER.SetCustomColor(FLinearColor::Yellow);
	RENDERER.SetOutlineState();

	mesh->GetVertexBuffer()->IASet();
	FMatrix outlineWorld = FMatrix::Scale({ 1.05f, 1.05f, 1.05f }) * transform.WorldMat;
	worldBuffer->SetMat(outlineWorld);	// 행렬 scale 높이기
	worldBuffer->SetVSBuffer(0);		// b0에 저장

	RENDERER.GetDeviceContext()->Draw(mesh->GetNumVertices(), 0);

	RENDERER.SetDefaultDepthState();
}

