#include "pch.h"
#include "AWorldAxises.h"
#include "FVertexSimple.h"

AWorldAxises::AWorldAxises()
{
	// 오브젝트 매니저를 통해 축선 메시 캐시 사용
	SetMesh(OBJECT.GetOrCreateMesh("WorldAxises", worldAxisVertices));
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



