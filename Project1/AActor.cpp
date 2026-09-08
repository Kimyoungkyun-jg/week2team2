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

void AActor::Render()
{
	UObject::Render();

	SetWorldBuffer();

	if (mesh)
	{
		if (bIsSelected)
		{
			// 본체 렌더 및 스텐실 마킹
			RENDERER.SetSelectedState();
			mesh->SetColor(Color);
			mesh->Render();

			// 외곽선 렌더
			RENDERER.SetOutlineState();

			FMatrix S = FMatrix::Scale(transform.Scale * 1.04f);
			FMatrix R = FMatrix::RotationZ(transform.Rotation.z) * FMatrix::RotationX(transform.Rotation.x) * FMatrix::RotationY(transform.Rotation.y);
			FMatrix T = FMatrix::Translation(transform.Location);
			worldBuffer->SetMat(S * R * T);
			worldBuffer->SetVSBuffer(0);

			mesh->SetColor(FLinearColor(1.0f, 0.6f, 0.0f, 1.0f));
			mesh->Render();

			// 버퍼 및 깊이 복원
			SetWorldBuffer();
			RENDERER.SetDefaultDepthState();
		}
		else
		{
			mesh->SetColor(Color);
			mesh->Render();
		}
	}
}

void AActor::Update(float Deltatime)
{
	UObject::Update(Deltatime);
}

