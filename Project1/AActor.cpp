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

void AActor::Render()
{
	UObject::Render();

	worldBuffer->SetMat(transform.WorldMat);
	worldBuffer->SetVSBuffer(0);

	// 자체 버텍스 버퍼가 있으면 저장된 InputLayout으로 자동 바인딩 후 렌더링
	if (vertexbuffer != nullptr && numVertices > 0)
	{
		REDERER.PrepareShader(inputLayout);
		vertexbuffer->IASet();
		REDERER.GetDeviceContext()->Draw(numVertices, 0);
	}
}



void AActor::Update(float Deltatime)
{
	UObject::Update(Deltatime);
}
