#include "pch.h"
#include "AWorldAxises.h"
#include "FVertexSimple.h"

AWorldAxises::AWorldAxises()
{
	InitVertexBuffer(worldAxisVertices);
	SetPrimitive(EPrimitive::None);

}

void AWorldAxises::Render()
{
	UObject::Render();

	SetWorldBuffer();

	RENDERER.SetCustomColor({ 0.0f, 0.0f, 0.0f, 0.0f });

	if (mesh)
	{
		mesh->Render(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	}
}



