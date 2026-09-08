#include "pch.h"
#include "Mesh.h"
#include "Intersection.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
    if (vertexbuffer)
    {
        delete vertexbuffer;
        vertexbuffer = nullptr;
    }
}

void Mesh::InitVertexBuffer(const void* vertices, UINT stride, UINT inNumVertices, ID3D11InputLayout* inLayout)
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

void Mesh::Render()
{
	if (vertexbuffer != nullptr && numVertices > 0)
	{
		RENDERER.PrepareShader(inputLayout);
		RENDERER.SetCustomColor(CurrentColor);
		vertexbuffer->IASet();
		RENDERER.GetDeviceContext()->Draw(numVertices, 0);
	}
}

void Mesh::Render(const FLinearColor& color)
{
	CurrentColor = color;
	Render();
}

void Mesh::Render(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	if (vertexbuffer != nullptr && numVertices > 0)
	{
		RENDERER.PrepareShader(inputLayout);
		RENDERER.SetCustomColor(CurrentColor);
		vertexbuffer->IASet(topology);
		RENDERER.GetDeviceContext()->Draw(numVertices, 0);
	}
}

void Mesh::Render(const FLinearColor& color, D3D11_PRIMITIVE_TOPOLOGY topology)
{
	CurrentColor = color;
	Render(topology);
}



void Mesh::IASet(D3D11_PRIMITIVE_TOPOLOGY type)
{
	if (vertexbuffer)
	{
		vertexbuffer->IASet(type);
	}
}

bool Mesh::bIsPicked(const FRay& worldRay, const Transform& transform, float& outDistance)
{
	if (LocalVertices.size() < 3) return false;

	if (vertexbuffer != nullptr && numVertices > 0)
	{
		// 비균등 스케일 및 회전에서도 오차가 없는 정확한 4x4 역행렬 사용
		FMatrix invWorld = transform.WorldMat.Inverse();
		FVector localOrigin = TransformPoint(worldRay.Origin, invWorld);
		FVector localDir = TransformDirection(worldRay.Direction, invWorld);
		

		float closestDist = FLT_MAX;
		bool bHit = false;

		// 삼각형 충돌 검사
		for (size_t i = 0; i + 2 < LocalVertices.size(); i += 3)
		{
			float dist = 0.0f;
			if (RayIntersectTriangle(localOrigin, localDir, LocalVertices[i], LocalVertices[i + 1], LocalVertices[i + 2], dist))
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
			// localDir을 정규화하지 않았으므로 closestDist가 곧바로 정확한 월드 거리
			outDistance = closestDist;


			return true;
		}
		return false;
	}

	return false;
}