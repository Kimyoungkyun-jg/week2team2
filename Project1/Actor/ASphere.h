#include "Sphere.h"

class ASphere : public AActor
{
	// ASphere 부모 AActor -> ACollider로 변경
	DECLARE_CLASS(ASphere, ACollider);

public:
	ASphere() {
		// 구체 정점 최초 계산 후 캐시
		static const std::vector<FVertexColor> sphereVertices = CreateSphereVertices(0.5f, 20, 20, false);
		SetMesh(OBJECT.GetOrCreateMesh("Sphere", sphereVertices));
	}


};

