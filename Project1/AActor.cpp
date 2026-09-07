#include "pch.h"
#include "AActor.h"
#include "Renderer.h"
#include "PickingManager.h"
#include "Intersection.h"

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

void AActor::Render()
{
	UObject::Render();

	worldBuffer->SetMat(transform.WorldMat);
	worldBuffer->SetVSBuffer(0);

	//버텍스 버퍼 바인딩 및 렌더링
	if (vertexbuffer != nullptr && numVertices > 0)
	{
		RENDERER.PrepareShader(inputLayout);
		RENDERER.SetCustomColor(Color);
		vertexbuffer->IASet();
		RENDERER.GetDeviceContext()->Draw(numVertices, 0);
	}
}



void AActor::Update(float Deltatime)
{
	UObject::Update(Deltatime);
}
