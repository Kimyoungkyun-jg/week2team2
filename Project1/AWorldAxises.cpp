#include "pch.h"
#include "AWorldAxises.h"
#include "FVertexSimple.h"

AWorldAxises::AWorldAxises()
{
	InitVertexBuffer(worldAxisVertices);
}

void AWorldAxises::Render()
{
	UObject::Render();

	SetWorldBuffer();

	RENDERER.PrepareShader(inputLayout);
	RENDERER.SetCustomColor({ 0.0f, 0.0f, 0.0f, 0.0f });

	vertexbuffer->IASet(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	DC->Draw(numVertices, 0);
}


