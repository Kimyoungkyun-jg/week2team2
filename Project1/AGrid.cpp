#include "pch.h"
#include "AGrid.h"

AGrid::AGrid(EGridType InType)
	: GridType(InType)
{
	CreateVertices();
	InitVertexBuffer(vertices);
}

void AGrid::CreateVertices()
{
	vertices.clear();

	if (GridType == EGridType::Line)
	{
		CreateLineVertices();
	}
	else
	{
		CreateTriangleVertices();
	}
}

void AGrid::CreateLineVertices()
{
	int halfW = (int)(width / 2);
	int halfH = (int)(height / 2);

	//X축 방향 평행선
	for (int z = -halfH; z <= halfH; z++)
	{
		vertices.emplace_back((float)-halfW, 0.0f, (float)z, 0.65f, 0.65f, 0.65f, 1.0f);
		vertices.emplace_back((float)+halfW, 0.0f, (float)z, 0.65f, 0.65f, 0.65f, 1.0f);
	}

	//Z축 방향 평행선
	for (int x = -halfW; x <= halfW; x++)
	{
		vertices.emplace_back((float)x, 0.0f, (float)-halfH, 0.65f, 0.65f, 0.65f, 1.0f);
		vertices.emplace_back((float)x, 0.0f, (float)+halfH, 0.65f, 0.65f, 0.65f, 1.0f);
	}
}

void AGrid::CreateTriangleVertices()
{
	int halfW = (int)(width / 2);
	int halfH = (int)(height / 2);

	vertices.reserve(width * height * 6);

	//격자 타일별 삼각형 면 생성
	for (int z = -halfH; z < halfH; z++)
	{
		for (int x = -halfW; x < halfW; x++)
		{
			float c = ((x + z) % 2 == 0) ? 0.68f : 0.58f;

			float fx = (float)x;
			float fz = (float)z;

			//삼각형 1
			vertices.emplace_back(fx,        0.0f, fz,        c, c, c, 1.0f);
			vertices.emplace_back(fx,        0.0f, fz + 1.0f, c, c, c, 1.0f);
			vertices.emplace_back(fx + 1.0f, 0.0f, fz + 1.0f, c, c, c, 1.0f);

			//삼각형 2
			vertices.emplace_back(fx,        0.0f, fz,        c, c, c, 1.0f);
			vertices.emplace_back(fx + 1.0f, 0.0f, fz + 1.0f, c, c, c, 1.0f);
			vertices.emplace_back(fx + 1.0f, 0.0f, fz,        c, c, c, 1.0f);
		}
	}
}

void AGrid::Render()
{
	UObject::Render();

	SetWorldBuffer();

	RENDERER.SetCustomColor({ 0.0f, 0.0f, 0.0f, 0.0f });

	if (mesh)
	{
		if (GridType == EGridType::Line)
		{
			mesh->Render(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		}
		else
		{
			mesh->Render(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
	}
}


