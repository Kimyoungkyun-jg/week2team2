#include "pch.h"
#include "AActor.h"
#include "Renderer.h"


AActor::AActor()
{
	worldBuffer = new MatrixBuffer();
	for (EGizmoAxis mode : { EGizmoAxis::X, EGizmoAxis::Z, EGizmoAxis::Y })
	{
		UGizmo* gizmo = new UGizmo(mode, this);
		gizmos.push_back(gizmo);
	}
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

	for (UGizmo* gizmo : gizmos)
	{
		delete gizmo;
	}
	gizmos.clear();
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
		RENDERER.PrepareShader(inputLayout);
		RENDERER.SetCustomColor(Color);
		vertexbuffer->IASet();
		RENDERER.GetDeviceContext()->Draw(numVertices, 0);
	}

	RenderGizmo();
}



void AActor::Update(float Deltatime)
{
	UObject::Update(Deltatime);

	UpdateGizmo(Deltatime);

}

void AActor::UpdateGizmo(float Deltatime)
{
	for (auto& it : gizmos)
	{
		it->Update(Deltatime);
	}
}

void AActor::RenderGizmo()
{
	for (auto& it : gizmos)
	{
		it->Render();
	}
}
