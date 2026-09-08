#include "pch.h"
#include "AActor.h"
#include "Renderer.h"
#include "PickingManager.h"
#include "Intersection.h"
#include "AGizmo.h"

AActor::AActor()
{
	worldBuffer = new MatrixBuffer();
}

AActor::~AActor()
{
	delete worldBuffer;
	worldBuffer = nullptr;

	if (vertexbuffer)
	{
		delete vertexbuffer;
		vertexbuffer = nullptr;
	}
}

void AActor::InitVertexBuffer(const void* vertices, UINT stride, UINT inNumVertices, ID3D11InputLayout* inLayout)
{
	numVertices = inNumVertices;
	inputLayout = inLayout;

	if (vertexbuffer)
	{
		delete vertexbuffer;
		vertexbuffer = nullptr;
	}


	if (vertices && inNumVertices > 0)
	{
		vertexbuffer = new VertexBuffer(vertices, stride, inNumVertices);
	}
}

bool AActor::bIsPicked(const FRay& worldRay, float& outDistance)
{
	if (LocalVertices.size() < 3) return false;

	if (vertexbuffer != nullptr && numVertices > 0)
	{
		FMatrix invWorld = transform.WorldMat.InverseAffine();
		FVector localOrigin = TransformPoint(worldRay.Origin, invWorld);
		FVector localDir = TransformDirection(worldRay.Direction, invWorld);
		localDir.Normalize();

		float closestDist = FLT_MAX;
		bool bHit = false;

		//삼각형 충돌 검사
		for (size_t i = 0; i + 2 < LocalVertices.size(); i += 3)
		{

			float dist = 0.0f;
			if (RayIntersectTriangle(localOrigin, localDir, LocalVertices[i], LocalVertices[i+1], LocalVertices[i+2], dist))
			{
				if (dist > 0.0f && dist < closestDist)
				{
					closestDist = dist;
					bHit = true;
				}
			}
		}

		if (bHit)
		{
			//월드 거리 보정
			outDistance = closestDist * transform.Scale.x;
			return true;
		}
		return false;
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
	//버텍스 버퍼 바인딩 및 렌더링
	if (vertexbuffer != nullptr && numVertices > 0)
	{
		//OutputDebugStringA("[AActor] DRAW ENTER\n");
		RENDERER.PrepareShader(inputLayout);

		RENDERER.SetCustomColor(Color);
		vertexbuffer->IASet();

		bool bSelected = IsSelected();
		if (bSelected) {
			RENDERER.SetSelectedState();
		}
			

		RENDERER.GetDeviceContext()->Draw(numVertices, 0);

		if (bSelected)
			RENDERER.SetDefaultDepthState();
	}
}

void AActor::RenderOutline()
{
	RENDERER.PrepareShader(inputLayout);
	RENDERER.SetCustomColor(FLinearColor::Yellow);
	RENDERER.SetOutlineState();

	vertexbuffer->IASet();
	FMatrix outlineWorld = FMatrix::Scale({ 1.05f, 1.05f, 1.05f }) * transform.WorldMat;
	worldBuffer->SetMat(outlineWorld);	// 행렬 scale 높이기
	worldBuffer->SetVSBuffer(0);		// b0에 저장

	RENDERER.GetDeviceContext()->Draw(numVertices, 0);

	RENDERER.SetDefaultDepthState();
}


void AActor::Update(float Deltatime)
{
	UObject::Update(Deltatime);
}

